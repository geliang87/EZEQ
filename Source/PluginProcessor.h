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
    
    enum FilterType
    {
        lowCutType,
        highCutType,
        bellType,
        notchType,
        bandPassType
    };
    
    float lowCutFreq = 100.0f, lowCutQ = 1.0f;
    FilterType filter2Type = FilterType::bellType;
    float filter2Freq = 200.0f, filter2Q = 0.707f, filter2Gain = 1.0f;
    FilterType filter3Type = FilterType::bellType;
    float filter3Freq = 400.0f, filter3Q = 0.707f, filter3Gain = 1.0f;
    FilterType filter4Type = FilterType::bellType;
    float filter4Freq = 800.0f, filter4Q = 0.707f, filter4Gain = 1.0f;
    FilterType filter5Type = FilterType::bellType;
    float filter5Freq = 1600.0f, filter5Q = 0.707f, filter5Gain = 1.0f;
    float highCutFreq = 20000.0f, highCutQ = 1.0f;
    
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
    
    void updateFilterSetup (int filterIndex, FilterType type, float freq, float Q, float gain);
    
    // Global gain parameter
    float gainIndB = 0.0f;
    
private:
    
    std::unique_ptr<Service::PresetManager> presetManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
