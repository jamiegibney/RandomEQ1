#pragma once
#include "PluginProcessor.h"
#include "RandomParameters.h"

using namespace juce;

class RandomEQEditor : public juce::AudioProcessorEditor {
 private:
    RandomEQProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RandomEQEditor)

    // UI elements

    Label freqLabel {{}, "Frequency" };
    ToggleButton freq125 { "125 Hz" };
    ToggleButton freq250 { "250 Hz" };
    ToggleButton freq500 { "500 Hz" };
    ToggleButton freq1k { "1 kHz" };
    ToggleButton freq3k { "3 kHz" };
    ToggleButton freq10k { "10 kHz" };

    Label gainLabel {{}, "Gain" };
    ToggleButton gainBoost { "Boost" };
    ToggleButton gainCut { "Cut" };
    ToggleButton gain1 { "1 dB" };
    ToggleButton gain3 { "3 dB" };
    ToggleButton gain6 { "6 dB" };
    ToggleButton gain12 { "12 dB" };

    TextButton check { "Check" };

    Label matchLabel {{}, "Select parameters..." };

    ToggleButton bypassFilter { "Bypass" };
    ToggleButton highQ { "High Q" };

    // Label coefTime {{}, "Filter processed in ---ns"};

    TooltipWindow ttw;

    RandomParameters currentParameters { 0.0f, 0.0f, Peak };

    void OnParameterMatch();
    void OnParameterMismatch();

    float chosenFreq {}, chosenGain;
    bool boostChosen = false;

    static constexpr u_int8_t shelfChance = 15;

public:
    explicit RandomEQEditor(RandomEQProcessor&);
    ~RandomEQEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    enum RadioGroupID {
        FreqOptions = 10001,
        GainOptions,
        GainBoostCut
    };

    void OnFreqClick(const float&);

    void OnGainClick(const float&);

    void OnBoostCutClick(const bool&);

    void OnCheckClick(ToggleButton&, ToggleButton&, ToggleButton&);

    void OnBypassClick(const bool&);

    void OnHighQClick(const bool&);

    RandomParameters eqRandom;
};

