#pragma once

#include <JuceHeader.h>
#include "XyloVoice.h"
#include "SampleLayer.h"

namespace xylocore
{
class XyloEngine
{
public:
    XyloEngine();

    void prepare (double sampleRate, int samplesPerBlock, int outputChannels);
    void render (juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void reset();
    void setParameters (const VoiceParameters& newParameters);
    bool loadSampleDirectory(const juce::File& directory);

    [[nodiscard]] int getActiveVoices() const;
    [[nodiscard]] std::array<float, 16> getVisualizerLevels() const;
    [[nodiscard]] juce::String getSampleStatusText() const;

private:
    void updateVisualizerFromMidi (const juce::MidiBuffer& midi);
    void propagateSharedState();

    juce::Synthesiser synth;
    VoiceParameters parameters;
    SampleLibraryPtr sampleLibrary;
    std::array<float, 16> visualizerLevels {};
};
} // namespace xylocore
