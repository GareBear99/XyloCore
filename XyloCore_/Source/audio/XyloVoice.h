#pragma once

#include <JuceHeader.h>
#include "SampleLayer.h"

namespace xylocore
{
struct VoiceParameters
{
    float tone = 0.62f;
    float hardness = 0.72f;
    float resonance = 0.55f;
    float damping = 0.42f;
    float air = 0.20f;
    float humanize = 0.18f;
    float width = 0.25f;
    float transient = 0.40f;
    float room = 0.12f;
    float material = 0.12f;
    float rollRate = 0.42f;
    float release = 0.35f;
    float shimmer = 0.18f;
    float octaveMix = 0.20f;
    float velocityCurve = 0.50f;
};

class XyloVoice final : public juce::SynthesiserVoice
{
public:
    XyloVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

    void prepare (double sampleRate, int samplesPerBlock, int outputChannels);
    void setVoiceParameters (const VoiceParameters& newParameters);
    void setSampleLibrary (SampleLibraryPtr newSampleLibrary);

private:
    struct Partial
    {
        float ratio = 1.0f;
        float gain = 0.0f;
        float decaySeconds = 0.2f;
        float baseDecaySeconds = 0.2f;
        double phase = 0.0;
        double phaseDelta = 0.0;
        float envelope = 0.0f;
    };

    void updatePartials (float frequency, float velocity);
    float renderProceduralSample();
    float renderSampleLayer();
    float randomCentered();

    std::array<Partial, 7> partials {};
    VoiceParameters parameters {};
    SampleLibraryPtr sampleLibrary;
    const LoadedSample* activeSample = nullptr;
    double sampleReadPosition = 0.0;
    double sampleReadDelta = 1.0;

    double currentSampleRate = 44100.0;
    int outputChannelCount = 2;
    float level = 0.0f;
    float tailOff = 0.0f;
    float transientEnv = 0.0f;
    float releaseBurstEnv = 0.0f;
    float shimmerState = 0.0f;
    float noiseState = 0.0f;
    float roomStateL = 0.0f;
    float roomStateR = 0.0f;
    float pan = 0.5f;
    float octaveEnv = 0.0f;
    double octavePhase = 0.0;
    double octavePhaseDelta = 0.0;
    juce::Random rng;
    juce::dsp::StateVariableTPTFilter<float> airFilter;
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
    juce::dsp::StateVariableTPTFilter<float> shimmerFilter;
};
} // namespace xylocore
