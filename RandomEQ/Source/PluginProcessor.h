#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Filter.h"
#include "RandomParameters.h"

class RandomEQProcessor : public juce::AudioProcessor {
 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RandomEQProcessor)

    static constexpr int channelCount = 2;

 public:
    RandomEQProcessor();
    ~RandomEQProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //                                  //                                  //

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    //                                  //                                  //

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //                                  //                                  //

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //                                  //                                  //

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    Filter filter[channelCount];
};
