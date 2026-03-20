#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>

namespace xylocore
{
struct LoadedSample
{
    juce::AudioBuffer<float> buffer;
    int rootMidi = 60;
    double sampleRate = 44100.0;
    juce::String sourceName;
};

class SampleLibrary
{
public:
    bool loadFromDirectory(const juce::File& directory);
    [[nodiscard]] const LoadedSample* getSampleForMidi(int midiNote) const;
    [[nodiscard]] int getLoadedSampleCount() const noexcept { return loadedCount; }
    [[nodiscard]] juce::String getStatusText() const { return statusText; }

private:
    static int midiFromFileName(const juce::String& fileNameWithoutExtension);

    std::array<std::unique_ptr<LoadedSample>, 128> noteSamples {};
    int loadedCount = 0;
    juce::String statusText { "Procedural mode" };
};

using SampleLibraryPtr = std::shared_ptr<SampleLibrary>;

} // namespace xylocore
