#include "PluginProcessor.h"
#include "PluginEditor.h"

RandomEQProcessor::RandomEQProcessor() : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                                                       ) {

}

RandomEQProcessor::~RandomEQProcessor() {
}

//                                    //                                    //

const juce::String RandomEQProcessor::getName() const {
    return JucePlugin_Name;
}

bool RandomEQProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RandomEQProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RandomEQProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RandomEQProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int RandomEQProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RandomEQProcessor::getCurrentProgram() {
    return 0;
}

void RandomEQProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused (index);
}

const juce::String RandomEQProcessor::getProgramName(int index) {
    juce::ignoreUnused (index);
    return {};
}

void RandomEQProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused (index, newName);
}

//                                    //                                    //

void RandomEQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (samplesPerBlock);

    for(auto& channel : filter) {
        channel.SetSampleRate((int)sampleRate);
    }
}

void RandomEQProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool RandomEQProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void RandomEQProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages) {
    ignoreUnused(midiMessages);

    // Clear unused output channels if there are less input channels (avoids garbage data)
    for(auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    /*
    Make sure to reset the state if your inner loop is processing
    the samples and the outer loop is handling the channels.
    Alternatively, you can process the samples with the channels
    interleaved by keeping the same state.
    */
    for(int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        const auto inputData = buffer.getReadPointer(channel);
        auto outputData = buffer.getWritePointer(channel);

        for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
            outputData[sample] = filter[channel].Process(inputData[sample] * 0.2f) * 5.0f;
        //                                                  may be redundant  ^       ^
    }
}

//                                    //                                    //

bool RandomEQProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RandomEQProcessor::createEditor() {
    return new RandomEQEditor(*this);
}

//                                    //                                    //

void RandomEQProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void RandomEQProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//                                    //                                    //

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new RandomEQProcessor();
}
