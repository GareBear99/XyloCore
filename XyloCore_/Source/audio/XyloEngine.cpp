#include "XyloEngine.h"
#include "XyloSound.h"

namespace xylocore
{
XyloEngine::XyloEngine()
    : sampleLibrary(std::make_shared<SampleLibrary>())
{
    for (int i = 0; i < 24; ++i)
        synth.addVoice(new XyloVoice());

    synth.addSound(new XyloSound());
    propagateSharedState();
}

void XyloEngine::prepare(double sampleRate, int samplesPerBlock, int outputChannels)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<XyloVoice*>(synth.getVoice(i)))
            voice->prepare(sampleRate, samplesPerBlock, outputChannels);

    propagateSharedState();
}

void XyloEngine::setParameters(const VoiceParameters& newParameters)
{
    parameters = newParameters;
    propagateSharedState();
}

bool XyloEngine::loadSampleDirectory(const juce::File& directory)
{
    const auto loaded = sampleLibrary->loadFromDirectory(directory);
    propagateSharedState();
    return loaded;
}

void XyloEngine::propagateSharedState()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<XyloVoice*>(synth.getVoice(i)))
        {
            voice->setVoiceParameters(parameters);
            voice->setSampleLibrary(sampleLibrary);
        }
    }
}

void XyloEngine::render(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    updateVisualizerFromMidi(midi);
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
    for (auto& level : visualizerLevels)
        level *= 0.947f;
}

void XyloEngine::reset()
{
    synth.allNotesOff(0, false);
}

int XyloEngine::getActiveVoices() const
{
    int active = 0;
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = synth.getVoice(i); voice != nullptr && voice->isVoiceActive())
            ++active;
    return active;
}

std::array<float, 16> XyloEngine::getVisualizerLevels() const
{
    return visualizerLevels;
}

juce::String XyloEngine::getSampleStatusText() const
{
    return sampleLibrary != nullptr ? sampleLibrary->getStatusText() : juce::String("Procedural mode");
}

void XyloEngine::updateVisualizerFromMidi(const juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            auto index = juce::jlimit(0, 15, msg.getNoteNumber() - 60);
            visualizerLevels[(size_t) index] = juce::jmax(visualizerLevels[(size_t) index], msg.getFloatVelocity());
        }
    }
}
} // namespace xylocore
