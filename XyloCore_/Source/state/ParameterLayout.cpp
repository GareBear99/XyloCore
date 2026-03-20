#include "ParameterLayout.h"

namespace xylocore
{
juce::AudioProcessorValueTreeState::ParameterLayout ParameterLayout::create()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto percent = [] (const juce::String& suffix)
    {
        return juce::AudioParameterFloatAttributes().withStringFromValueFunction(
            [suffix] (float value, int)
            {
                return juce::String(juce::roundToInt(value * 100.0f)) + suffix;
            });
    };

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        gainId, "Gain", juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), -6.0f,
        juce::AudioParameterFloatAttributes().withLabel(" dB")));

    auto add01 = [&] (const char* id, const char* name, float value)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            id, name, juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), value, percent("%")));
    };

    add01(toneId, "Tone", 0.62f);
    add01(hardnessId, "Hardness", 0.72f);
    add01(resonanceId, "Resonance", 0.55f);
    add01(dampingId, "Damping", 0.42f);
    add01(airId, "Air", 0.20f);
    add01(humanizeId, "Humanize", 0.18f);
    add01(widthId, "Width", 0.25f);
    add01(transientId, "Transient", 0.40f);
    add01(roomId, "Room", 0.12f);
    add01(materialId, "Material", 0.12f);
    add01(rollRateId, "Roll Rate", 0.42f);
    add01(releaseId, "Release", 0.35f);
    add01(shimmerId, "Shimmer", 0.18f);
    add01(octaveMixId, "Octave Mix", 0.20f);
    add01(velocityCurveId, "Velocity Curve", 0.50f);

    return { params.begin(), params.end() };
}
} // namespace xylocore
