#include "SampleLayer.h"
#include <regex>

namespace xylocore
{
namespace
{
int noteNameToPitchClass(const juce::String& name)
{
    const juce::String upper = name.toUpperCase();
    if (upper == "C") return 0;
    if (upper == "C#" || upper == "DB") return 1;
    if (upper == "D") return 2;
    if (upper == "D#" || upper == "EB") return 3;
    if (upper == "E") return 4;
    if (upper == "F") return 5;
    if (upper == "F#" || upper == "GB") return 6;
    if (upper == "G") return 7;
    if (upper == "G#" || upper == "AB") return 8;
    if (upper == "A") return 9;
    if (upper == "A#" || upper == "BB") return 10;
    if (upper == "B") return 11;
    return -1;
}
}

bool SampleLibrary::loadFromDirectory(const juce::File& directory)
{
    loadedCount = 0;
    statusText = "Procedural mode";
    for (auto& slot : noteSamples)
        slot.reset();

    if (! directory.isDirectory())
    {
        statusText = "Procedural mode · no assets/samples directory";
        return false;
    }

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    const auto files = directory.findChildFiles(juce::File::findFiles, false, "*.wav;*.aif;*.aiff;*.flac");
    for (const auto& file : files)
    {
        const auto midi = midiFromFileName(file.getFileNameWithoutExtension());
        if (! juce::isPositiveAndBelow(midi, 128))
            continue;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
        if (reader == nullptr || reader->lengthInSamples <= 0)
            continue;

        auto sample = std::make_unique<LoadedSample>();
        sample->buffer.setSize((int) reader->numChannels, (int) reader->lengthInSamples);
        reader->read(&sample->buffer, 0, (int) reader->lengthInSamples, 0, true, true);
        sample->rootMidi = midi;
        sample->sampleRate = reader->sampleRate;
        sample->sourceName = file.getFileName();
        noteSamples[(size_t) midi] = std::move(sample);
        ++loadedCount;
    }

    if (loadedCount > 0)
    {
        statusText = "Hybrid mode · " + juce::String(loadedCount) + " sample notes loaded";
        return true;
    }

    statusText = "Procedural mode · sample files not recognized";
    return false;
}

const LoadedSample* SampleLibrary::getSampleForMidi(int midiNote) const
{
    if (! juce::isPositiveAndBelow(midiNote, 128))
        return nullptr;

    if (noteSamples[(size_t) midiNote] != nullptr)
        return noteSamples[(size_t) midiNote].get();

    for (int radius = 1; radius < 12; ++radius)
    {
        const auto lower = midiNote - radius;
        const auto upper = midiNote + radius;
        if (juce::isPositiveAndBelow(lower, 128) && noteSamples[(size_t) lower] != nullptr)
            return noteSamples[(size_t) lower].get();
        if (juce::isPositiveAndBelow(upper, 128) && noteSamples[(size_t) upper] != nullptr)
            return noteSamples[(size_t) upper].get();
    }

    return nullptr;
}

int SampleLibrary::midiFromFileName(const juce::String& fileNameWithoutExtension)
{
    const std::string text = fileNameWithoutExtension.toStdString();
    std::smatch match;

    if (std::regex_match(text, match, std::regex(R"((\d{1,3}))")))
    {
        const auto midi = std::stoi(match[1].str());
        return juce::jlimit(0, 127, midi);
    }

    if (std::regex_match(text, match, std::regex(R"(([A-Ga-g])([#bB]?)(-?\d))")))
    {
        juce::String note = juce::String(match[1].str()) + juce::String(match[2].str());
        const int pitchClass = noteNameToPitchClass(note);
        if (pitchClass < 0)
            return -1;
        const int octave = std::stoi(match[3].str());
        return juce::jlimit(0, 127, (octave + 1) * 12 + pitchClass);
    }

    if (std::regex_search(text, match, std::regex(R"((\d{1,3}))")))
    {
        const auto midi = std::stoi(match[1].str());
        return juce::jlimit(0, 127, midi);
    }

    if (std::regex_search(text, match, std::regex(R"(([A-Ga-g])([#bB]?)(-?\d))")))
    {
        juce::String note = juce::String(match[1].str()) + juce::String(match[2].str());
        const int pitchClass = noteNameToPitchClass(note);
        if (pitchClass < 0)
            return -1;
        const int octave = std::stoi(match[3].str());
        return juce::jlimit(0, 127, (octave + 1) * 12 + pitchClass);
    }

    return -1;
}

} // namespace xylocore
