#include "PluginEditor.h"

RotarySlider::RotarySlider()
    : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow)
{
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
}

XyloCoreAudioProcessorEditor::XyloCoreAudioProcessorEditor (XyloCoreAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), keyboardBarView(p)
{
    setSize(1180, 840);

    titleLabel.setText("XyloCore", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(34.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::gold);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Flagship xylophone engine production foundation v0.3", juce::dontSendNotification);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.75f));
    addAndMakeVisible(subtitleLabel);

    statsLabel.setJustificationType(juce::Justification::centredRight);
    statsLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
    addAndMakeVisible(statsLabel);

    footerLabel.setText("Hybrid-ready architecture: procedural core now, premium samples next", juce::dontSendNotification);
    footerLabel.setJustificationType(juce::Justification::centredRight);
    footerLabel.setColour(juce::Label::textColourId, juce::Colours::gold.withAlpha(0.65f));
    addAndMakeVisible(footerLabel);

    sampleStatusLabel.setJustificationType(juce::Justification::centredLeft);
    sampleStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.72f));
    addAndMakeVisible(sampleStatusLabel);

    presetLabel.setText("Factory Preset", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.85f));
    addAndMakeVisible(presetLabel);

    presetBox.addListener(this);
    addAndMakeVisible(presetBox);
    prevPresetButton.onClick = [this]
    {
        auto idx = juce::jmax(0, presetBox.getSelectedItemIndex() - 1);
        presetBox.setSelectedItemIndex(idx, juce::sendNotification);
    };
    nextPresetButton.onClick = [this]
    {
        auto idx = juce::jmin(presetBox.getNumItems() - 1, presetBox.getSelectedItemIndex() + 1);
        presetBox.setSelectedItemIndex(idx, juce::sendNotification);
    };
    addAndMakeVisible(prevPresetButton);
    addAndMakeVisible(nextPresetButton);

    addAndMakeVisible(keyboardBarView);

    configureSlider(gain, "Gain", xylocore::ParameterLayout::gainId);
    configureSlider(tone, "Tone", xylocore::ParameterLayout::toneId);
    configureSlider(hardness, "Hardness", xylocore::ParameterLayout::hardnessId);
    configureSlider(resonance, "Resonance", xylocore::ParameterLayout::resonanceId);
    configureSlider(damping, "Damping", xylocore::ParameterLayout::dampingId);
    configureSlider(air, "Air", xylocore::ParameterLayout::airId);
    configureSlider(humanize, "Humanize", xylocore::ParameterLayout::humanizeId);
    configureSlider(width, "Width", xylocore::ParameterLayout::widthId);
    configureSlider(transient, "Transient", xylocore::ParameterLayout::transientId);
    configureSlider(room, "Room", xylocore::ParameterLayout::roomId);
    configureSlider(material, "Material", xylocore::ParameterLayout::materialId);
    configureSlider(rollRate, "Roll Rate", xylocore::ParameterLayout::rollRateId);
    configureSlider(release, "Release", xylocore::ParameterLayout::releaseId);
    configureSlider(shimmer, "Shimmer", xylocore::ParameterLayout::shimmerId);
    configureSlider(octaveMix, "Octave Mix", xylocore::ParameterLayout::octaveMixId);
    configureSlider(velocityCurve, "Velocity Curve", xylocore::ParameterLayout::velocityCurveId);

    refreshPresetBox();
    startTimerHz(20);
}

void XyloCoreAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient bg(juce::Colour(0xff070707), 0.0f, 0.0f,
                            juce::Colour(0xff25160c), (float) getWidth(), (float) getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();

    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 22.0f);

    auto hero = juce::Rectangle<float>(24.0f, 82.0f, (float) getWidth() - 48.0f, 292.0f);
    juce::ColourGradient glow(juce::Colours::gold.withAlpha(0.13f), hero.getCentreX(), hero.getY() + 12.0f,
                              juce::Colours::transparentBlack, hero.getCentreX(), hero.getBottom(), false);
    g.setGradientFill(glow);
    g.fillRoundedRectangle(hero, 20.0f);

    auto sliderField = juce::Rectangle<float>(24.0f, 394.0f, (float) getWidth() - 48.0f, 390.0f);
    juce::ColourGradient sliderGlow(juce::Colours::gold.withAlpha(0.05f), sliderField.getCentreX(), sliderField.getY(),
                                    juce::Colour(0x00111111), sliderField.getCentreX(), sliderField.getBottom(), false);
    g.setGradientFill(sliderGlow);
    g.fillRoundedRectangle(sliderField, 20.0f);

    g.setColour(juce::Colours::gold.withAlpha(0.16f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 22.0f, 1.0f);
}

void XyloCoreAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    auto top = area.removeFromTop(46);
    titleLabel.setBounds(top.removeFromLeft(230));
    subtitleLabel.setBounds(top.removeFromLeft(480));
    statsLabel.setBounds(top);

    auto presetArea = area.removeFromTop(44);
    presetLabel.setBounds(presetArea.removeFromLeft(120));
    prevPresetButton.setBounds(presetArea.removeFromLeft(28).reduced(2));
    presetBox.setBounds(presetArea.removeFromLeft(320).reduced(4));
    nextPresetButton.setBounds(presetArea.removeFromLeft(28).reduced(2));
    auto sampleArea = presetArea.removeFromLeft(280);
    sampleStatusLabel.setBounds(sampleArea.reduced(4, 0));
    footerLabel.setBounds(presetArea.reduced(4, 0));

    keyboardBarView.setBounds(area.removeFromTop(300).reduced(0, 6));
    area.removeFromTop(12);

    auto placeRow = [&] (std::initializer_list<SliderPack*> row, int height)
    {
        auto rowArea = area.removeFromTop(height);
        auto w = rowArea.getWidth() / (int) row.size();
        for (auto* pack : row)
        {
            auto cell = rowArea.removeFromLeft(w).reduced(4);
            pack->label.setBounds(cell.removeFromTop(22));
            pack->slider.setBounds(cell);
        }
        area.removeFromTop(6);
    };

    placeRow({ &gain, &tone, &hardness, &resonance, &damping, &air }, 140);
    placeRow({ &humanize, &width, &transient, &room, &material, &rollRate }, 140);
    placeRow({ &release, &shimmer, &octaveMix, &velocityCurve }, 140);
}

void XyloCoreAudioProcessorEditor::timerCallback()
{
    statsLabel.setText("Active voices: " + juce::String(audioProcessor.getEngine().getActiveVoices())
                       + "  |  Click bars or send MIDI", juce::dontSendNotification);
    sampleStatusLabel.setText(audioProcessor.getSampleStatusText(), juce::dontSendNotification);
}

void XyloCoreAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetBox)
        audioProcessor.loadFactoryPresetByIndex(presetBox.getSelectedItemIndex());
}

void XyloCoreAudioProcessorEditor::configureSlider(SliderPack& pack, const juce::String& name, const juce::String& paramId)
{
    pack.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::gold.withAlpha(0.85f));
    pack.slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    pack.slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    pack.slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    pack.slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff1b1b1b));
    addAndMakeVisible(pack.slider);

    pack.label.setText(name, juce::dontSendNotification);
    pack.label.setJustificationType(juce::Justification::centred);
    pack.label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.86f));
    addAndMakeVisible(pack.label);

    pack.attachment = std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), paramId, pack.slider);
}

void XyloCoreAudioProcessorEditor::refreshPresetBox()
{
    presetBox.clear();
    auto names = audioProcessor.getFactoryPresetNames();
    int id = 1;
    for (const auto& name : names)
        presetBox.addItem(name, id++);

    if (presetBox.getNumItems() > 0)
        presetBox.setSelectedItemIndex(0, juce::dontSendNotification);
}
