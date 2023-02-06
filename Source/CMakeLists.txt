# Initialised CMakeLists.txt file for a JUCE audio plugin.

cmake_minimum_required(VERSION 3.15)

project(RandomEQ VERSION 0.0.1)

#            FIND JUCE           #

# (usually best to include this within the source folder, if using)
add_subdirectory(<juce_path> [binary])

# Optionally, use FetchContent to get JUCE from GitHub:
# include(FetchContent)
# FetchContent_Declare(
#        JUCE
#        GIT_REPOSITORY https://github.com/juce-framework/JUCE/
#        GIT_TAG 7.0.5)
# FetchContent_MakeAvailable(JUCE)

                #

# SDKs for VST2 (Steinberg) or AAX (Avid) formats
# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

# All the "metadata" of the plugin — more arguments are available
# See https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md for the full list

juce_add_plugin(RandomEQ
#    VERSION ...                               # Set this if the plugin version is different to the project version
#    ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
#    ICON_SMALL ...
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
    COMPANY_NAME jamiegibney                    # Specify the name of the plugin's author
    PLUGIN_MANUFACTURER_CODE Jvst               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dem0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS VST3 Standalone                    # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "RandomEQ")        # The name of the final executable, which can differ from the target name

# Use this to create a "global" juce header — for when you don't want to individually include modules.
# juce_generate_juce_header(RandomEQ)

# The target is listed first, followed by any source files you wish to add to it (with a visibility modifier)
target_sources(RandomEQ
    PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
        Filter.cpp
        RandomParameters.cpp)

                #

# EXTRA COMPILER/LINKER OPTIONS

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(RandomEQ
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(RandomEQ
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
