cmake_minimum_required(VERSION 3.22)
project(XyloCore VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(XYLOCORE_USE_LOCAL_JUCE "Use JUCE from modules/JUCE" ON)
option(XYLOCORE_USE_FETCHCONTENT "Download JUCE with FetchContent when local JUCE is unavailable" OFF)

if (XYLOCORE_USE_LOCAL_JUCE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules/JUCE/CMakeLists.txt")
    add_subdirectory(modules/JUCE)
elseif (XYLOCORE_USE_FETCHCONTENT)
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.8
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

juce_add_plugin(XyloCore
    COMPANY_NAME "Gary Doman"
    BUNDLE_ID com.garydoman.xylocore
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE GDom
    PLUGIN_CODE XyC4
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "XyloCore")

juce_generate_juce_header(XyloCore)

target_sources(XyloCore PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/state/ParameterLayout.cpp
    Source/state/PresetManager.cpp
    Source/audio/XyloEngine.cpp
    Source/audio/XyloVoice.cpp
    Source/audio/XyloSound.cpp
    Source/audio/SampleLayer.cpp
    Source/ui/KeyboardBarView.cpp
)

target_include_directories(XyloCore PRIVATE
    Source
    Source/audio
    Source/state
    Source/ui
)

target_compile_definitions(XyloCore PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(XyloCore PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

if (APPLE)
    set_target_properties(XyloCore_Standalone PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "XyloCore")
endif()
