# XyloCore

XyloCore is a JUCE-based xylophone instrument plugin with a production-ready project scaffold, a polished custom UI, embedded factory presets, and a hybrid engine path.

## Current state

This package is the strongest version so far:
- JUCE CMake plugin project
- AU / VST3 / Standalone targets
- procedural xylophone core that makes sound immediately
- hybrid sample layer support from `assets/samples/`
- 16 automatable parameters
- embedded and JSON-overridable factory presets
- clickable xylophone bar UI plus MIDI input
- save/restore state via APVTS
- GitHub Actions starter workflow for macOS builds

## What is actually complete

This package is ready to be opened in a JUCE/CMake workflow and moved into real host testing.

It includes:
- compile-oriented source layout
- build scripts
- preset system
- hybrid fallback behavior
- sample-folder scanning
- production-minded defaults

## What is not honestly finished yet

The true "best xylophone plugin on the planet" release would still need:
- a premium recorded sample library with consistent mic capture
- multiple velocity layers per note
- round robins and alternate-hit variation from recordings
- a deeper performance engine for rolls/trills/gliss phrases
- full DAW validation across real host environments
- code signing, installer packaging, and release QA

So this is a serious release foundation, not a fake claim of a final commercial finish.

## Sample folder support

Drop note samples into:

`assets/samples/`

Recognized file naming examples:
- `60.wav`
- `C4.wav`
- `F#5.wav`
- `xylophone_C5.wav`
- `note_72.wav`

The loader will try to map exact note files first, then nearby fallback notes. If no usable files are found, the plugin stays in procedural mode.

## Build options

### Option 1: local JUCE checkout

Put JUCE at:

`modules/JUCE`

Then run:

```bash
cmake --preset default
cmake --build --preset default --config Release -j
```

### Option 2: FetchContent build

```bash
cmake --preset fetchcontent
cmake --build --preset fetchcontent --config Release -j
```

### Option 3: helper script on macOS

```bash
./scripts/build_macos.sh
```

## Suggested validation path

1. Build Standalone first.
2. Confirm mouse-click bars and MIDI both trigger notes.
3. Confirm presets change state and restore after relaunch.
4. Add a few real samples to `assets/samples/` and verify status switches to hybrid mode.
5. Test AU in Logic / GarageBand and VST3 in a host like Reaper.
6. Stress-test polyphony and session recall.

## Source overview

- `Source/PluginProcessor.*` plugin state and routing
- `Source/PluginEditor.*` UI and attachments
- `Source/audio/XyloEngine.*` synth engine wrapper
- `Source/audio/XyloVoice.*` hybrid procedural/sample voice
- `Source/audio/SampleLayer.*` sample directory scanning and note mapping
- `Source/state/PresetManager.*` embedded + JSON presets
- `Source/ui/KeyboardBarView.*` xylophone bar control surface

## Recommended next pushes

The highest-value next moves are:
- real recorded xylophone library
- velocity-layer aware sample mapping
- per-note round robin slots
- dedicated roll/trill/gliss performance page
- FX rack and preset browser polish
- host-tested release packaging
