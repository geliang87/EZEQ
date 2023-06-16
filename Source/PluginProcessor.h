/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetManager.h"

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor,
public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void parameterChanged (const String &parameterID, float newValue) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
    Service::PresetManager& getPresetManager() { return *presetManager; }
    
    using Filter = juce::dsp::IIR::Filter<float>;
    dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter> leftChain, rightChain;
    float lowCutFreq = 100.0f, lowCutQ = 1.0f, highCutFreq = 20000.0f, highCutQ = 1.0f;
    float filter2Freq, filter2Q, filter2Gain;
    float filter3Freq, filter3Q, filter3Gain;
    float filter4Freq, filter4Q, filter4Gain;
    float filter5Freq, filter5Q, filter5Gain;
    
    enum ChainPosition
    {
        lowCut,
        filter2,
        filter3,
        filter4,
        filter5,
        highCut
    };
    
    void updateCoefficients (Filter::CoefficientsPtr &old, const Filter::CoefficientsPtr &replacements)
    {
        *old = *replacements;
    }
    
private:
    
    std::unique_ptr<Service::PresetManager> presetManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
