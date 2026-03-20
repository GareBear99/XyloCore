#include "PresetManager.h"
#include "ParameterLayout.h"

namespace xylocore
{
namespace
{
juce::var parsePresetText(const juce::String& text)
{
    return juce::JSON::parse(text);
}
}

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& stateToManage)
    : apvts(stateToManage)
{
}

juce::File PresetManager::getFactoryPresetDirectory() const
{
    auto cwd = juce::File::getCurrentWorkingDirectory();
    auto direct = cwd.getChildFile("assets").getChildFile("presets");
    if (direct.isDirectory())
        return direct;

    auto parent = cwd.getParentDirectory().getChildFile("assets").getChildFile("presets");
    if (parent.isDirectory())
        return parent;

    return {};
}

juce::Array<PresetManager::PresetEntry> PresetManager::getEmbeddedFactoryPresets()
{
    juce::Array<PresetEntry> entries;
    auto add = [&entries](juce::String name, juce::String json)
    {
        PresetEntry e;
        e.name = std::move(name);
        e.embeddedJson = std::move(json);
        entries.add(std::move(e));
    };

    add("Arcade Crystal", R"json({"name":"Arcade Crystal","gain":-6.8,"tone":0.78,"hardness":0.68,"resonance":0.64,"damping":0.22,"air":0.36,"humanize":0.15,"width":0.60,"transient":0.43,"room":0.24,"material":0.90,"rollRate":0.56,"release":0.45,"shimmer":0.80,"octaveMix":0.40,"velocityCurve":0.66})json");
    add("Bright Soloist", R"json({"name":"Bright Soloist","gain":-6.0,"tone":0.76,"hardness":0.82,"resonance":0.46,"damping":0.44,"air":0.23,"humanize":0.14,"width":0.34,"transient":0.66,"room":0.14,"material":0.24,"rollRate":0.43,"release":0.34,"shimmer":0.16,"octaveMix":0.18,"velocityCurve":0.62})json");
    add("Concert Natural", R"json({"name":"Concert Natural","gain":-6.0,"tone":0.58,"hardness":0.48,"resonance":0.58,"damping":0.35,"air":0.18,"humanize":0.18,"width":0.28,"transient":0.32,"room":0.12,"material":0.15,"rollRate":0.40,"release":0.32,"shimmer":0.10,"octaveMix":0.12,"velocityCurve":0.55})json");
    add("Dream Glass", R"json({"name":"Dream Glass","gain":-8.0,"tone":0.69,"hardness":0.59,"resonance":0.67,"damping":0.18,"air":0.39,"humanize":0.17,"width":0.62,"transient":0.27,"room":0.36,"material":0.82,"rollRate":0.35,"release":0.58,"shimmer":0.72,"octaveMix":0.34,"velocityCurve":0.58})json");
    add("Indie Music Box", R"json({"name":"Indie Music Box","gain":-8.5,"tone":0.61,"hardness":0.37,"resonance":0.49,"damping":0.39,"air":0.21,"humanize":0.24,"width":0.44,"transient":0.23,"room":0.19,"material":0.54,"rollRate":0.30,"release":0.46,"shimmer":0.35,"octaveMix":0.31,"velocityCurve":0.52})json");
    add("Lo Fi Wood", R"json({"name":"Lo Fi Wood","gain":-7.5,"tone":0.36,"hardness":0.30,"resonance":0.44,"damping":0.55,"air":0.08,"humanize":0.28,"width":0.18,"transient":0.20,"room":0.06,"material":0.00,"rollRate":0.33,"release":0.26,"shimmer":0.02,"octaveMix":0.08,"velocityCurve":0.40})json");
    add("Noir Hall", R"json({"name":"Noir Hall","gain":-7.0,"tone":0.52,"hardness":0.41,"resonance":0.61,"damping":0.23,"air":0.20,"humanize":0.20,"width":0.58,"transient":0.25,"room":0.42,"material":0.10,"rollRate":0.31,"release":0.62,"shimmer":0.18,"octaveMix":0.20,"velocityCurve":0.50})json");
    add("Pixel Glint", R"json({"name":"Pixel Glint","gain":-7.2,"tone":0.74,"hardness":0.71,"resonance":0.57,"damping":0.36,"air":0.30,"humanize":0.16,"width":0.47,"transient":0.49,"room":0.15,"material":0.63,"rollRate":0.52,"release":0.33,"shimmer":0.52,"octaveMix":0.22,"velocityCurve":0.64})json");
    add("Soft Mallets", R"json({"name":"Soft Mallets","gain":-7.0,"tone":0.44,"hardness":0.22,"resonance":0.63,"damping":0.26,"air":0.14,"humanize":0.22,"width":0.22,"transient":0.18,"room":0.10,"material":0.05,"rollRate":0.38,"release":0.42,"shimmer":0.06,"octaveMix":0.10,"velocityCurve":0.48})json");
    add("Stage Attack", R"json({"name":"Stage Attack","gain":-5.5,"tone":0.84,"hardness":0.94,"resonance":0.40,"damping":0.58,"air":0.26,"humanize":0.12,"width":0.32,"transient":0.91,"room":0.10,"material":0.28,"rollRate":0.48,"release":0.24,"shimmer":0.08,"octaveMix":0.12,"velocityCurve":0.74})json");
    add("Trailer Spark", R"json({"name":"Trailer Spark","gain":-5.0,"tone":0.81,"hardness":0.90,"resonance":0.52,"damping":0.41,"air":0.34,"humanize":0.11,"width":0.55,"transient":0.82,"room":0.28,"material":0.42,"rollRate":0.50,"release":0.38,"shimmer":0.38,"octaveMix":0.26,"velocityCurve":0.70})json");
    add("Warm Felt Bell", R"json({"name":"Warm Felt Bell","gain":-7.4,"tone":0.47,"hardness":0.26,"resonance":0.66,"damping":0.29,"air":0.18,"humanize":0.19,"width":0.24,"transient":0.16,"room":0.14,"material":0.11,"rollRate":0.34,"release":0.51,"shimmer":0.12,"octaveMix":0.09,"velocityCurve":0.46})json");

    std::sort(entries.begin(), entries.end(), [] (const PresetEntry& a, const PresetEntry& b)
    {
        return a.name.compareNatural(b.name) < 0;
    });

    return entries;
}

juce::Array<PresetManager::PresetEntry> PresetManager::scanFactoryPresets() const
{
    auto entries = getEmbeddedFactoryPresets();
    auto dir = getFactoryPresetDirectory();
    if (! dir.isDirectory())
        return entries;

    for (const auto& file : dir.findChildFiles(juce::File::findFiles, false, "*.json"))
    {
        auto presetName = file.getFileNameWithoutExtension().replaceCharacter('_', ' ');
        bool replacedEmbedded = false;

        for (auto& existing : entries)
        {
            if (existing.name == presetName)
            {
                existing.file = file;
                existing.embeddedJson.clear();
                replacedEmbedded = true;
                break;
            }
        }

        if (! replacedEmbedded)
        {
            PresetEntry e;
            e.file = file;
            e.name = presetName;
            entries.add(std::move(e));
        }
    }

    std::sort(entries.begin(), entries.end(), [] (const PresetEntry& a, const PresetEntry& b)
    {
        return a.name.compareNatural(b.name) < 0;
    });

    return entries;
}

juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray names;
    for (const auto& entry : scanFactoryPresets())
        names.add(entry.name);
    return names;
}

bool PresetManager::loadFactoryPresetByIndex(int index)
{
    auto entries = scanFactoryPresets();
    if (! juce::isPositiveAndBelow(index, entries.size()))
        return false;

    return loadFactoryPresetByName(entries.getReference(index).name);
}

bool PresetManager::loadFactoryPresetByName(const juce::String& name)
{
    for (const auto& entry : scanFactoryPresets())
    {
        if (entry.name == name)
        {
            juce::var parsed;
            if (entry.file.existsAsFile())
                parsed = juce::JSON::parse(entry.file);
            else
                parsed = parsePresetText(entry.embeddedJson);

            return ! parsed.isVoid() && applyJsonObject(parsed);
        }
    }

    return false;
}

bool PresetManager::applyJsonObject(const juce::var& jsonRoot)
{
    auto* object = jsonRoot.getDynamicObject();
    if (object == nullptr)
        return false;

    auto setFloat = [&] (const juce::Identifier& id)
    {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(id.toString())))
            if (object->hasProperty(id))
                param->setValueNotifyingHost(param->convertTo0to1((float) object->getProperty(id)));
    };

    if (auto* gainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(ParameterLayout::gainId)))
        if (object->hasProperty(ParameterLayout::gainId))
            gainParam->setValueNotifyingHost(gainParam->convertTo0to1((float) object->getProperty(ParameterLayout::gainId)));

    setFloat(ParameterLayout::toneId);
    setFloat(ParameterLayout::hardnessId);
    setFloat(ParameterLayout::resonanceId);
    setFloat(ParameterLayout::dampingId);
    setFloat(ParameterLayout::airId);
    setFloat(ParameterLayout::humanizeId);
    setFloat(ParameterLayout::widthId);
    setFloat(ParameterLayout::transientId);
    setFloat(ParameterLayout::roomId);
    setFloat(ParameterLayout::materialId);
    setFloat(ParameterLayout::rollRateId);
    setFloat(ParameterLayout::releaseId);
    setFloat(ParameterLayout::shimmerId);
    setFloat(ParameterLayout::octaveMixId);
    setFloat(ParameterLayout::velocityCurveId);
    return true;
}
} // namespace xylocore
