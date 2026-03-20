#pragma once

#include <JuceHeader.h>

namespace xylocore
{
struct ParameterLayout
{
    static juce::AudioProcessorValueTreeState::ParameterLayout create();

    static constexpr auto gainId = "gain";
    static constexpr auto toneId = "tone";
    static constexpr auto hardnessId = "hardness";
    static constexpr auto resonanceId = "resonance";
    static constexpr auto dampingId = "damping";
    static constexpr auto airId = "air";
    static constexpr auto humanizeId = "humanize";
    static constexpr auto widthId = "width";
    static constexpr auto transientId = "transient";
    static constexpr auto roomId = "room";
    static constexpr auto materialId = "material";
    static constexpr auto rollRateId = "rollRate";
    static constexpr auto releaseId = "release";
    static constexpr auto shimmerId = "shimmer";
    static constexpr auto octaveMixId = "octaveMix";
    static constexpr auto velocityCurveId = "velocityCurve";
};
} // namespace xylocore
