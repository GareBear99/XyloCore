> 🎛️ Part of the [TizWildin Plugin Ecosystem](https://garebear99.github.io/TizWildinEntertainmentHUB/) — 16 free audio plugins with a live update dashboard.
>
> [FreeEQ8](https://github.com/GareBear99/FreeEQ8) · [XyloCore](https://github.com/GareBear99/XyloCore) · [Instrudio](https://github.com/GareBear99/Instrudio) · [Therum](https://github.com/GareBear99/Therum_JUCE-Plugin) · [BassMaid](https://github.com/GareBear99/BassMaid) · [SpaceMaid](https://github.com/GareBear99/SpaceMaid) · [GlueMaid](https://github.com/GareBear99/GlueMaid) · [MixMaid](https://github.com/GareBear99/MixMaid) · [ChainMaid](https://github.com/GareBear99/ChainMaid) · [PaintMask](https://github.com/GareBear99/PaintMask_Free-JUCE-Plugin) · [WURP](https://github.com/GareBear99/WURP_Toxic-Motion-Engine_JUCE) · [AETHER](https://github.com/GareBear99/AETHER_Choir-Atmosphere-Designer) · [WhisperGate](https://github.com/GareBear99/WhisperGate_Free-JUCE-Plugin) · [RiftWave](https://github.com/GareBear99/RiftWaveSuite_RiftSynth_WaveForm_Lite) · [FreeSampler](https://github.com/GareBear99/FreeSampler_v0.3) · [VF-PlexLab](https://github.com/GareBear99/VF-PlexLab) · [PAP-Forge-Audio](https://github.com/GareBear99/PAP-Forge-Audio)
>
> 🎁 [Free Packs & Samples](#tizwildin-free-sample-packs) — jump to free packs & samples
>
> 🎵 [Awesome Audio](https://github.com/GareBear99/awesome-audio-plugins-dev) — (FREE) Awesome Audio Dev List

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

## TizWildin FREE sample packs

| Pack | Description |
|------|-------------|
| [**TizWildin-Aurora**](https://github.com/GareBear99/TizWildin-Aurora) | 3-segment original synth melody pack with loops, stems, demo renders, and neon/cinematic phrasing |
| [**TizWildin-Obsidian**](https://github.com/GareBear99/TizWildin-Obsidian) | Dark cinematic sample pack with choir textures, menu loops, transitions, bass, atmosphere, drums, and electric-banjo extensions |
| [**TizWildin-Skyline**](https://github.com/GareBear99/TizWildin-Skyline) | 30 BPM-tagged synthwave and darkwave loops with generator snapshot and dark neon additions |
| [**TizWildin-Chroma**](https://github.com/GareBear99/TizWildin-Chroma) | Multi-segment game synthwave loop sample pack from TizWildin Entertainment |
| [**TizWildin-Chime**](https://github.com/GareBear99/TizWildin-Chime) | Multi-part 88 BPM chime collection spanning glass, void, halo, reed, and neon synthwave lanes |
| [**Free Violin Synth Sample Kit**](https://github.com/GareBear99/Free-Violin-Synth-Sample-Kit) | Physical-model violin sample kit rendered from the Instrudio violin instrument |
| [**Free Dark Piano Sound Kit**](https://github.com/GareBear99/Free-Dark-Piano-Sound-Kit) | 88 piano notes + dark/cinematic loops and MIDI |
| [**Free 808 Producer Kit**](https://github.com/GareBear99/Free-808-Producer-Kit) | 94 hand-crafted 808 bass samples tuned to every chromatic key |
| [**Free Riser Producer Kit**](https://github.com/GareBear99/Free-Riser-Producer-Kit) | 115+ risers and 63 downlifters - noise, synth, drum, FX, cinematic |
| [**Phonk Producer Toolkit**](https://github.com/GareBear99/Phonk_Producer_Toolkit) | Drift phonk starter kit - 808s, cowbells, drums, MIDI, templates |
| [**Free Future Bass Producer Kit**](https://github.com/GareBear99/Free-Future-Bass-Producer-Kit) | Loops, fills, drums, bass, synths, pads, and FX |

### Related audio projects
- [**VF-PlexLab**](https://github.com/GareBear99/VF-PlexLab) - VocalForge PersonaPlex Lab starter repo for a JUCE plugin + local backend + HTML tester around NVIDIA PersonaPlex.
- [**PAP-Forge-Audio**](https://github.com/GareBear99/PAP-Forge-Audio) - Procedural Autonomous Plugins runtime for generating, branching, validating, and restoring plugin projects from natural-language sound intent.
