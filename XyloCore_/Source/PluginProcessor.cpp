#include "PluginProcessor.h"
#include "PluginEditor.h"

XyloCoreAudioProcessor::XyloCoreAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", xylocore::ParameterLayout::create()),
      presetManager(apvts)
{
    presetManager.loadFactoryPresetByName("Concert Natural");
}

namespace
{
juce::File findSampleDir()
{
    auto cwd = juce::File::getCurrentWorkingDirectory();
    auto direct = cwd.getChildFile("assets").getChildFile("samples");
    if (direct.isDirectory())
        return direct;

    auto parent = cwd.getParentDirectory().getChildFile("assets").getChildFile("samples");
    if (parent.isDirectory())
        return parent;

    return {};
}
}

void XyloCoreAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    gainSmoothed.reset(sampleRate, 0.05);
    gainSmoothed.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(
        apvts.getRawParameterValue(xylocore::ParameterLayout::gainId)->load()));

    engine.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    engine.setParameters(getVoiceParameters());
    engine.loadSampleDirectory(findSampleDir());
}

void XyloCoreAudioProcessor::releaseResources()
{
    engine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool XyloCoreAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void XyloCoreAudioProcessor::enqueueMidiMessage(const juce::MidiMessage& message)
{
    const juce::ScopedLock lock(pendingMidiLock);
    pendingMidi.addEvent(message, 0);
}

juce::StringArray XyloCoreAudioProcessor::getFactoryPresetNames() const
{
    return presetManager.getFactoryPresetNames();
}

bool XyloCoreAudioProcessor::loadFactoryPresetByIndex(int index)
{
    return presetManager.loadFactoryPresetByIndex(index);
}

juce::String XyloCoreAudioProcessor::getSampleStatusText() const
{
    return engine.getSampleStatusText();
}

void XyloCoreAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    {
        const juce::ScopedLock lock(pendingMidiLock);
        midiMessages.addEvents(pendingMidi, 0, buffer.getNumSamples(), 0);
        pendingMidi.clear();
    }

    buffer.clear();
    engine.setParameters(getVoiceParameters());
    engine.render(buffer, midiMessages);

    auto gainTarget = juce::Decibels::decibelsToGain(
        apvts.getRawParameterValue(xylocore::ParameterLayout::gainId)->load());
    gainSmoothed.setTargetValue(gainTarget);

    auto startGain = gainSmoothed.getCurrentValue();
    auto endGain = startGain;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        endGain = gainSmoothed.getNextValue();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), startGain, endGain);
}

juce::AudioProcessorEditor* XyloCoreAudioProcessor::createEditor()
{
    return new XyloCoreAudioProcessorEditor(*this);
}

void XyloCoreAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void XyloCoreAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

xylocore::VoiceParameters XyloCoreAudioProcessor::getVoiceParameters() const
{
    xylocore::VoiceParameters parameters;
    auto get = [&] (auto id)
    {
        return apvts.getRawParameterValue(id)->load();
    };

    parameters.tone = get(xylocore::ParameterLayout::toneId);
    parameters.hardness = get(xylocore::ParameterLayout::hardnessId);
    parameters.resonance = get(xylocore::ParameterLayout::resonanceId);
    parameters.damping = get(xylocore::ParameterLayout::dampingId);
    parameters.air = get(xylocore::ParameterLayout::airId);
    parameters.humanize = get(xylocore::ParameterLayout::humanizeId);
    parameters.width = get(xylocore::ParameterLayout::widthId);
    parameters.transient = get(xylocore::ParameterLayout::transientId);
    parameters.room = get(xylocore::ParameterLayout::roomId);
    parameters.material = get(xylocore::ParameterLayout::materialId);
    parameters.rollRate = get(xylocore::ParameterLayout::rollRateId);
    parameters.release = get(xylocore::ParameterLayout::releaseId);
    parameters.shimmer = get(xylocore::ParameterLayout::shimmerId);
    parameters.octaveMix = get(xylocore::ParameterLayout::octaveMixId);
    parameters.velocityCurve = get(xylocore::ParameterLayout::velocityCurveId);
    return parameters;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XyloCoreAudioProcessor();
}
