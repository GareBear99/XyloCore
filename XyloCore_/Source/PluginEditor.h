#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/KeyboardBarView.h"

class RotarySlider final : public juce::Slider
{
public:
    RotarySlider();
};

class XyloCoreAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer,
                                           private juce::ComboBox::Listener
{
public:
    explicit XyloCoreAudioProcessorEditor (XyloCoreAudioProcessor&);
    ~XyloCoreAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    struct SliderPack
    {
        RotarySlider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    void timerCallback() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void configureSlider (SliderPack& pack, const juce::String& name, const juce::String& paramId);
    void refreshPresetBox();

    XyloCoreAudioProcessor& audioProcessor;
    KeyboardBarView keyboardBarView;
    juce::Label titleLabel, subtitleLabel, statsLabel, footerLabel, sampleStatusLabel;
    juce::ComboBox presetBox;
    juce::Label presetLabel;
    juce::TextButton prevPresetButton { "<" }, nextPresetButton { ">" };

    SliderPack gain, tone, hardness, resonance, damping, air;
    SliderPack humanize, width, transient, room, material, rollRate;
    SliderPack release, shimmer, octaveMix, velocityCurve;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XyloCoreAudioProcessorEditor)
};
