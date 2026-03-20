#pragma once

#include <JuceHeader.h>
#include "audio/XyloEngine.h"
#include "state/ParameterLayout.h"
#include "state/PresetManager.h"

class XyloCoreAudioProcessor final : public juce::AudioProcessor
{
public:
    XyloCoreAudioProcessor();
    ~XyloCoreAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    xylocore::XyloEngine& getEngine() { return engine; }
    xylocore::PresetManager& getPresetManager() { return presetManager; }

    void enqueueMidiMessage (const juce::MidiMessage& message);
    juce::StringArray getFactoryPresetNames() const;
    bool loadFactoryPresetByIndex(int index);
    juce::String getSampleStatusText() const;

private:
    xylocore::VoiceParameters getVoiceParameters() const;

    juce::AudioProcessorValueTreeState apvts;
    xylocore::XyloEngine engine;
    xylocore::PresetManager presetManager;
    juce::LinearSmoothedValue<float> gainSmoothed;
    juce::CriticalSection pendingMidiLock;
    juce::MidiBuffer pendingMidi;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XyloCoreAudioProcessor)
};
