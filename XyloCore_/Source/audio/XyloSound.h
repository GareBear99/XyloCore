#pragma once

#include <JuceHeader.h>

namespace xylocore
{
class XyloSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};
} // namespace xylocore
