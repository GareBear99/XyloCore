#include "XyloVoice.h"
#include "XyloSound.h"

namespace xylocore
{
namespace
{
constexpr double twoPi = juce::MathConstants<double>::twoPi;
}

XyloVoice::XyloVoice()
{
    partials = {{
        { 1.00f, 0.62f, 1.25f, 1.25f },
        { 3.99f, 0.22f, 0.82f, 0.82f },
        { 9.20f, 0.14f, 0.54f, 0.54f },
        { 16.6f, 0.10f, 0.32f, 0.32f },
        { 24.0f, 0.06f, 0.20f, 0.20f },
        { 31.3f, 0.04f, 0.16f, 0.16f },
        { 38.4f, 0.03f, 0.13f, 0.13f }
    }};
}

bool XyloVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<XyloSound*>(sound) != nullptr;
}

void XyloVoice::prepare(double sampleRate, int, int outputChannels)
{
    currentSampleRate = sampleRate;
    outputChannelCount = juce::jmax(1, outputChannels);

    juce::dsp::ProcessSpec spec { sampleRate, 512, 1 };
    airFilter.reset();
    toneFilter.reset();
    shimmerFilter.reset();
    airFilter.prepare(spec);
    toneFilter.prepare(spec);
    shimmerFilter.prepare(spec);
    airFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    toneFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    shimmerFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
}

void XyloVoice::setVoiceParameters(const VoiceParameters& newParameters)
{
    parameters = newParameters;

    auto toneHz = juce::jmap(parameters.tone, 0.0f, 1.0f, 1800.0f, 10500.0f);
    toneFilter.setCutoffFrequency(toneHz + parameters.material * 1400.0f);

    auto airHz = juce::jmap(parameters.air, 0.0f, 1.0f, 4500.0f, 11000.0f);
    airFilter.setCutoffFrequency(airHz);

    shimmerFilter.setCutoffFrequency(juce::jmap(parameters.shimmer, 0.0f, 1.0f, 2200.0f, 8200.0f));
    shimmerFilter.setResonance(juce::jmap(parameters.shimmer, 0.0f, 1.0f, 0.5f, 2.4f));
}

void XyloVoice::setSampleLibrary(SampleLibraryPtr newSampleLibrary)
{
    sampleLibrary = std::move(newSampleLibrary);
}

void XyloVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    auto baseFreq = (float) juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto detuneCents = (randomCentered() * parameters.humanize) * 7.0f;
    auto detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);
    auto frequency = baseFreq * detuneRatio;

    auto curve = juce::jmap(parameters.velocityCurve, 0.0f, 1.0f, 0.55f, 1.9f);
    auto shapedVelocity = std::pow(juce::jlimit(0.001f, 1.0f, velocity), curve);
    auto velHumanize = juce::jlimit(0.05f, 1.0f, shapedVelocity + randomCentered() * parameters.humanize * 0.08f);

    level = velHumanize;
    tailOff = 1.0f;
    transientEnv = juce::jmap(parameters.transient, 0.0f, 1.0f, 0.04f, 0.28f) * (0.35f + velHumanize * 0.65f);
    releaseBurstEnv = 0.0f;
    shimmerState = 0.0f;
    pan = juce::jlimit(0.05f, 0.95f, 0.5f + randomCentered() * parameters.width * 0.35f);
    roomStateL = roomStateR = 0.0f;

    octavePhase = 0.0;
    octavePhaseDelta = twoPi * frequency * 2.0 / currentSampleRate;
    octaveEnv = velHumanize * parameters.octaveMix * 0.25f;

    activeSample = sampleLibrary != nullptr ? sampleLibrary->getSampleForMidi(midiNoteNumber) : nullptr;
    sampleReadPosition = 0.0;
    sampleReadDelta = 1.0;
    if (activeSample != nullptr)
    {
        const auto sampleNoteHz = juce::MidiMessage::getMidiNoteInHertz(activeSample->rootMidi);
        sampleReadDelta = (baseFreq / sampleNoteHz) * (activeSample->sampleRate / currentSampleRate);
    }

    updatePartials(frequency, velHumanize);
}

void XyloVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        releaseBurstEnv = juce::jmax(releaseBurstEnv, 0.03f + parameters.release * 0.18f);
        if (tailOff == 0.0f)
            tailOff = 1.0f;
    }
    else
    {
        clearCurrentNote();
        tailOff = 0.0f;
        activeSample = nullptr;
    }
}

void XyloVoice::updatePartials(float frequency, float velocity)
{
    auto hardnessScale = juce::jmap(parameters.hardness, 0.0f, 1.0f, 0.65f, 1.50f);
    auto dampingScale = juce::jmap(parameters.damping, 0.0f, 1.0f, 0.65f, 1.75f);
    auto materialBright = juce::jmap(parameters.material, 0.0f, 1.0f, 0.95f, 1.35f);
    auto resonanceScale = juce::jmap(parameters.resonance, 0.0f, 1.0f, 0.75f, 1.45f);

    for (auto& partial : partials)
    {
        partial.phase = 0.0;
        partial.phaseDelta = twoPi * frequency * partial.ratio / currentSampleRate;
        partial.envelope = partial.gain * velocity;

        if (partial.ratio > 8.0f)
            partial.envelope *= hardnessScale * materialBright;
        else
            partial.envelope *= resonanceScale;

        partial.decaySeconds = juce::jmax(0.04f, partial.baseDecaySeconds * (2.0f - dampingScale));
    }
}

float XyloVoice::renderProceduralSample()
{
    float tonal = 0.0f;
    for (auto& partial : partials)
    {
        tonal += std::sin(partial.phase) * partial.envelope;
        partial.phase += partial.phaseDelta;
        partial.envelope *= std::exp(-1.0f / (partial.decaySeconds * (float) currentSampleRate));
    }

    auto octave = std::sin(octavePhase) * octaveEnv;
    octavePhase += octavePhaseDelta;
    octaveEnv *= 0.99955f;

    auto noise = randomCentered();
    noiseState = 0.84f * noiseState + 0.16f * noise;
    auto transient = transientEnv * noiseState;
    transientEnv *= juce::jmap(parameters.hardness, 0.0f, 1.0f, 0.91f, 0.80f);

    releaseBurstEnv *= juce::jmap(parameters.release, 0.0f, 1.0f, 0.90f, 0.97f);
    auto releaseNoise = releaseBurstEnv * (noiseState * 0.5f + randomCentered() * 0.5f);

    auto combined = tonal + transient + octave + releaseNoise;
    auto airy = airFilter.processSample(combined) * parameters.air * 0.42f;
    auto shimmer = shimmerFilter.processSample(airy + releaseNoise * 0.3f);
    shimmerState = 0.92f * shimmerState + 0.08f * shimmer;
    auto body = toneFilter.processSample(combined + airy + shimmerState * parameters.shimmer * 0.5f);

    return body + airy * 0.3f + shimmerState * parameters.shimmer * 0.4f;
}

float XyloVoice::renderSampleLayer()
{
    if (activeSample == nullptr)
        return 0.0f;

    const auto numChannels = activeSample->buffer.getNumChannels();
    const auto numSamples = activeSample->buffer.getNumSamples();
    if (numChannels <= 0 || numSamples <= 1)
        return 0.0f;

    const auto index = (int) sampleReadPosition;
    if (index < 0 || index >= numSamples - 1)
        return 0.0f;

    const auto frac = (float) (sampleReadPosition - (double) index);
    float mix = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const auto* read = activeSample->buffer.getReadPointer(ch);
        const auto a = read[index];
        const auto b = read[index + 1];
        mix += juce::jmap(frac, a, b);
    }
    mix /= (float) numChannels;
    sampleReadPosition += sampleReadDelta;
    return mix;
}

void XyloVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    auto leftGain = std::sqrt(1.0f - pan);
    auto rightGain = std::sqrt(pan);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto procedural = renderProceduralSample();
        const auto sampled = renderSampleLayer();
        const auto hybridMix = activeSample != nullptr ? 0.62f : 0.0f;
        auto dry = ((procedural * (1.0f - hybridMix)) + (sampled * hybridMix)) * level * tailOff;

        roomStateL = 0.985f * roomStateL + dry * (0.05f + parameters.room * 0.08f);
        roomStateR = 0.983f * roomStateR + dry * (0.04f + parameters.room * 0.09f);
        auto wetL = roomStateL * parameters.room * 0.35f;
        auto wetR = roomStateR * parameters.room * 0.35f;

        outputBuffer.addSample(0, startSample + sample, dry * leftGain + wetL);
        if (outputChannelCount > 1)
            outputBuffer.addSample(1, startSample + sample, dry * rightGain + wetR);

        if (tailOff > 0.0f)
            tailOff *= juce::jmap(parameters.release, 0.0f, 1.0f, 0.99986f, 0.999985f);

        if (std::abs(dry) < 0.00003f && transientEnv < 0.00005f && releaseBurstEnv < 0.00005f
            && roomStateL < 0.00005f && roomStateR < 0.00005f)
        {
            clearCurrentNote();
            activeSample = nullptr;
            break;
        }
    }
}

float XyloVoice::randomCentered()
{
    return rng.nextFloat() * 2.0f - 1.0f;
}
} // namespace xylocore
