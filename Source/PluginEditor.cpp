#include "PluginProcessor.h"
#include "PluginEditor.h"

RandomEQEditor::RandomEQEditor(RandomEQProcessor& p)
                  : AudioProcessorEditor (&p), processorRef (p) {
    juce::ignoreUnused (processorRef);

    // frequency options
    freqLabel.setFont(18.0f);

    addAndMakeVisible(&freqLabel);

    freq125.setRadioGroupId(FreqOptions);
    freq250.setRadioGroupId(FreqOptions);
    freq500.setRadioGroupId(FreqOptions);
    freq1k.setRadioGroupId(FreqOptions);
    freq3k.setRadioGroupId(FreqOptions);
    freq10k.setRadioGroupId(FreqOptions);

    freq125.onClick = [&] { OnFreqClick(125.0f); };
    freq250.onClick = [&] { OnFreqClick(250.0f); };
    freq500.onClick = [&] { OnFreqClick(500.0f); };
    freq1k.onClick = [&] { OnFreqClick(1000.0f); };
    freq3k.onClick = [&] { OnFreqClick(3000.0f); };
    freq10k.onClick = [&] { OnFreqClick(10000.0f); };

    addAndMakeVisible(&freq125);
    addAndMakeVisible(&freq250);
    addAndMakeVisible(&freq500);
    addAndMakeVisible(&freq1k);
    addAndMakeVisible(&freq3k);
    addAndMakeVisible(&freq10k);

    // gain options
    gainLabel.setFont(18.0f);

    addAndMakeVisible(&gainLabel);

    gainBoost.setRadioGroupId(GainBoostCut);
    gainCut.setRadioGroupId(GainBoostCut);

    gainBoost.onClick = [&] { OnBoostCutClick(true); };
    gainCut.onClick = [&] { OnBoostCutClick(false); };

    addAndMakeVisible(&gainBoost);
    addAndMakeVisible(&gainCut);

    gain1.setRadioGroupId(GainOptions);
    gain3.setRadioGroupId(GainOptions);
    gain6.setRadioGroupId(GainOptions);
    gain12.setRadioGroupId(GainOptions);

    gain1.onClick = [&] { OnGainClick(1.0f); };
    gain3.onClick = [&] { OnGainClick(3.0f); };
    gain6.onClick = [&] { OnGainClick(6.0f); };
    gain12.onClick = [&] { OnGainClick(12.0f); };

    addAndMakeVisible(&gain1);
    addAndMakeVisible(&gain3);
    addAndMakeVisible(&gain6);
    addAndMakeVisible(&gain12);

    // check button
    check.onClick = [&] { OnCheckClick(freq125, gainBoost, gain1); };
    check.setTooltip("Set what you think the filter parameters are, then click here to check");

    addAndMakeVisible(&check);

    // match label
    matchLabel.setFont(18.0f);

    addAndMakeVisible(&matchLabel);

    // processorRef.randomFilter = &eqRandom;

    // bypass button
    bypassFilter.setToggleable(true);
    bypassFilter.onClick = [&] { OnBypassClick(bypassFilter.getToggleState()); };

    addAndMakeVisible(&bypassFilter);

    // high Q button
    highQ.setToggleable(true);
    highQ.onClick = [&] { OnHighQClick(highQ.getToggleState()); };
    highQ.setTooltip("Use higher Q values to accentuate filter resonance");

    addAndMakeVisible(&highQ);

    // coefTime.setFont(13.0f);
    // coefTime.setJustificationType(Justification::centred);
    // coefTime.setTooltip("The time taken to calculate the new filter in nanoseconds");
    //
    // addAndMakeVisible(&coefTime);

    setSize(500, 300);
}

RandomEQEditor::~RandomEQEditor() {
}

void RandomEQEditor::OnFreqClick(const float& freqVal) {
    chosenFreq = freqVal;
}

void RandomEQEditor::OnGainClick(const float& gainVal) {
    chosenGain = gainVal;
}

void RandomEQEditor::OnBoostCutClick(const bool& isBoost) {
    chosenGain = isBoost ? abs(chosenGain) : -abs(chosenGain);
    boostChosen = isBoost;
}

void RandomEQEditor::OnCheckClick(ToggleButton& freq, ToggleButton& gainBoostCut,
                                  ToggleButton& gain) {
    freq.triggerClick();
    gainBoostCut.triggerClick();
    gain.triggerClick();

    if(chosenFreq == eqRandom.mFreq && chosenGain == eqRandom.mGain)
        OnParameterMatch();
    else
        OnParameterMismatch();

    eqRandom.Randomise(0);

    for(auto& channel : processorRef.filter)
        channel.SetParameters(eqRandom.mType, eqRandom.mFreq,
                              channel.mQ, eqRandom.mGain);

    matchLabel.setBounds(250, getHeight() / 2 + 30, 250, 100);

    // int coefTimeMean = (processorRef.filter[0].GetCoefficientProcessTime() +
    //                     processorRef.filter[0].GetCoefficientProcessTime()) / 2;
    //
    //
    // coefTime.setText("Filter processed in " + std::to_string(coefTimeMean) + "ns",
    //                  NotificationType::dontSendNotification);
}

void RandomEQEditor::OnBypassClick(const bool& buttonState) {
    for(auto& channel : processorRef.filter) {
        channel.mEnabled = !buttonState;
    }
}

void RandomEQEditor::OnHighQClick(const bool& buttonState) {
    for(auto& channel : processorRef.filter) {
        channel.mQ = buttonState ? 3.5 : 0.7;
    }
}

void RandomEQEditor::paint(juce::Graphics& g) {
    // Fill the background with a solid colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void RandomEQEditor::OnParameterMatch() {
    matchLabel.setText("Correct!", NotificationType::dontSendNotification);
}

void RandomEQEditor::OnParameterMismatch() {
    String typeText;

    switch(eqRandom.mType) {
        case LowShelf:
            typeText = "low-shelf";
            break;
        case HighShelf:
            typeText = "high-shelf";
            break;
        case Peak:
            typeText = "peak";
            break;
    }

    String gainText = (eqRandom.mGain > 0 ? "+" : "") +
        std::to_string((int)eqRandom.mGain) + " dB";

    String freqText;

    if(eqRandom.mFreq >= 1000.0f)
        freqText = std::to_string((int)eqRandom.mFreq / 1000) + " kHz";
    else
        freqText = std::to_string((int)eqRandom.mFreq) + " Hz";

    // String labelText = "Nope, the EQ band was:\nA " + typeText + " filter\nwith " +
    //     gainText + " of gain\nat " + freqText;

    String labelText = "Nope, the parameters were:\n" + freqText + "  \t  |  \t  " + gainText;

    matchLabel.setText(labelText, NotificationType::dontSendNotification);
}

void RandomEQEditor::resized() {
    int gainXPos = 175;
    int buttonYSpace = 40;

    freqLabel.setJustificationType(Justification::centredLeft);
    freqLabel.setBounds(30, 5, 120, 30);
    gainLabel.setJustificationType(Justification::centredLeft);
    gainLabel.setBounds(gainXPos, 5, 80, 30);
    matchLabel.setJustificationType(Justification::centred);
    matchLabel.setBounds(290, getHeight() / 2 + 30, 200, 100);

    freq125.setBounds(30, buttonYSpace * 1, 80, 30);
    freq250.setBounds(30, buttonYSpace * 2, 80, 30);
    freq500.setBounds(30, buttonYSpace * 3, 80, 30);
    freq1k.setBounds(30, buttonYSpace * 4, 80, 30);
    freq3k.setBounds(30, buttonYSpace * 5, 80, 30);
    freq10k.setBounds(30, buttonYSpace * 6, 80, 30);

    gainBoost.setBounds(gainXPos, buttonYSpace, 80, 30);
    gainCut.setBounds(gainXPos + 75, buttonYSpace, 80, 30);

    gain1.setBounds(gainXPos, buttonYSpace * 2, 80, 30);
    gain3.setBounds(gainXPos, buttonYSpace * 3, 80, 30);
    gain6.setBounds(gainXPos, buttonYSpace * 4, 80, 30);
    gain12.setBounds(gainXPos, buttonYSpace * 5, 80, 30);

    freq125.triggerClick();
    gainBoost.triggerClick();
    gain1.triggerClick();
    highQ.triggerClick();

    check.setBounds(350, getHeight() / 2 - 15, 80, 30);

    bypassFilter.setBounds(350, buttonYSpace, 80, 30);

    highQ.setBounds(350, buttonYSpace * 2, 80, 30);

    // coefTime.setBounds(getWidth() / 2 - 125, buttonYSpace * 6.65, 250, 30);
}
