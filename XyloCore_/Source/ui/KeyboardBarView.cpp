#include "KeyboardBarView.h"
#include "../PluginProcessor.h"

KeyboardBarView::KeyboardBarView (XyloCoreAudioProcessor& processor)
    : audioProcessor(processor)
{
    startTimerHz(30);
}

void KeyboardBarView::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    g.fillAll(juce::Colour(0xff0f0f0f));

    auto levels = audioProcessor.getEngine().getVisualizerLevels();
    auto barWidth = bounds.getWidth() / 16.0f;

    for (int i = 0; i < 16; ++i)
    {
        auto midi = 60 + i;
        auto noteName = juce::MidiMessage::getMidiNoteName(midi, true, true, 3);
        auto barHeight = juce::jmap((float) i, 0.0f, 15.0f, bounds.getHeight() * 0.95f, bounds.getHeight() * 0.62f);
        auto x = bounds.getX() + i * barWidth;
        auto y = bounds.getCentreY() - barHeight * 0.5f;
        auto barBounds = juce::Rectangle<float>(x + 4.0f, y, barWidth - 8.0f, barHeight);

        auto glow = juce::jlimit(0.0f, 1.0f, levels[(size_t) i]);
        auto base = juce::Colour(0xff8b5a2b).interpolatedWith(juce::Colours::gold, 0.22f + glow * 0.55f);

        g.setColour(base.withBrightness(0.55f + glow * 0.35f));
        g.fillRoundedRectangle(barBounds, 8.0f);

        if (glow > 0.02f)
        {
            g.setColour(juce::Colours::gold.withAlpha(glow * 0.55f));
            g.drawRoundedRectangle(barBounds.expanded(2.5f), 9.5f, 2.0f);
        }

        g.setColour(juce::Colours::black.withAlpha(0.25f));
        g.drawRoundedRectangle(barBounds, 8.0f, 1.0f);

        g.setColour(juce::Colours::white.withAlpha(0.75f));
        g.setFont(12.0f);
        g.drawText(noteName, barBounds.removeFromBottom(24.0f).toNearestInt(), juce::Justification::centred, false);
    }
}

void KeyboardBarView::mouseDown (const juce::MouseEvent& event)
{
    triggerNoteForPosition(event.position, 1.0f);
}

void KeyboardBarView::mouseUp (const juce::MouseEvent&)
{
    if (activeMouseNote >= 0)
    {
        audioProcessor.enqueueMidiMessage(juce::MidiMessage::noteOff(1, activeMouseNote));
        activeMouseNote = -1;
    }
}

void KeyboardBarView::mouseDrag (const juce::MouseEvent& event)
{
    triggerNoteForPosition(event.position, juce::jlimit(0.3f, 1.0f, 1.0f - event.position.y / (float) getHeight()));
}

void KeyboardBarView::timerCallback()
{
    repaint();
}

int KeyboardBarView::positionToMidiNote (juce::Point<float> position) const
{
    auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    auto normalized = juce::jlimit(0.0f, 0.999f, (position.x - bounds.getX()) / bounds.getWidth());
    return 60 + (int) std::floor(normalized * 16.0f);
}

void KeyboardBarView::triggerNoteForPosition (juce::Point<float> position, float velocityScale)
{
    auto note = positionToMidiNote(position);

    if (note != activeMouseNote)
    {
        if (activeMouseNote >= 0)
            audioProcessor.enqueueMidiMessage(juce::MidiMessage::noteOff(1, activeMouseNote));

        activeMouseNote = note;
        audioProcessor.enqueueMidiMessage(juce::MidiMessage::noteOn(1, activeMouseNote,
            (juce::uint8) juce::jlimit(1, 127, (int) std::round(velocityScale * 127.0f))));
    }
}
