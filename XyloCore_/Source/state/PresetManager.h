#pragma once

#include <JuceHeader.h>

namespace xylocore
{
class PresetManager
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& stateToManage);

    juce::StringArray getFactoryPresetNames() const;
    bool loadFactoryPresetByIndex(int index);
    bool loadFactoryPresetByName(const juce::String& name);

private:
    struct PresetEntry
    {
        juce::String name;
        juce::File file;
        juce::String embeddedJson;
    };

    juce::File getFactoryPresetDirectory() const;
    juce::Array<PresetEntry> scanFactoryPresets() const;
    static juce::Array<PresetEntry> getEmbeddedFactoryPresets();
    bool applyJsonObject(const juce::var& jsonRoot);

    juce::AudioProcessorValueTreeState& apvts;
};
} // namespace xylocore
