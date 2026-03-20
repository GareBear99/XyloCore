#pragma once

#include <JuceHeader.h>

class XyloCoreAudioProcessor;

class KeyboardBarView final : public juce::Component,
                              private juce::Timer
{
public:
    explicit KeyboardBarView (XyloCoreAudioProcessor& processor);
    ~KeyboardBarView() override = default;

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    int positionToMidiNote (juce::Point<float> position) const;
    void triggerNoteForPosition (juce::Point<float> position, float velocityScale);

    XyloCoreAudioProcessor& audioProcessor;
    int activeMouseNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyboardBarView)
};
