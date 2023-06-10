/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetManager.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    apvts.state.setProperty(Service::PresetManager::presetNameProperty,"", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);
    presetManager = std::make_unique<Service::PresetManager>(apvts);
    
    apvts.addParameterListener ("Scale", this);
    apvts.addParameterListener ("Gain", this);
    
    for (int i = 1; i <= 6; ++i)
    {
        String bypassString ("Bypass");
        bypassString << i;
        
        String typeString ("Type");
        typeString << i;
        
        String freqString ("Freq");
        freqString << i;
        
        String gainString ("Gain");
        gainString << i;
        
        String QString ("Q");
        QString << i;
        
        apvts.addParameterListener (bypassString, this);
        apvts.addParameterListener (typeString, this);
        apvts.addParameterListener (freqString, this);
        apvts.addParameterListener (gainString, this);
        apvts.addParameterListener (QString, this);
    }
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    
    spec.numChannels = 1;
    
    spec.sampleRate = sampleRate;
    
    leftChain.prepare (spec);
    rightChain.prepare (spec);
    
    // 配置滤波器默认值
    // lowCut
    auto& leftLowCut = leftChain.get<ChainPosition::lowCut>();
    auto& rightLowCut = rightChain.get<ChainPosition::lowCut>();
    
    auto newCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), lowCutFreq, lowCutQ);
    
    updateCoefficients (leftLowCut.coefficients, newCoefficients);
    updateCoefficients (rightLowCut.coefficients, newCoefficients);
    
    // filter2
    // filter3
    // filter4
    // filter5
    // highCut
}

void SimpleEQAudioProcessor::releaseResources()
{
    leftChain.reset();
    rightChain.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (//layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    return new SimpleEQAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void SimpleEQAudioProcessor::parameterChanged (const String &parameterID, float newValue)
{
    DBG (parameterID);
    DBG (std::to_string (newValue));
    
    if (parameterID.startsWith ("Freq"))
    {
        auto lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                auto& leftLowCut = leftChain.get<ChainPosition::lowCut>();
                auto& rightLowCut = rightChain.get<ChainPosition::lowCut>();
                
                lowCutFreq = newValue;
                auto newCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), lowCutFreq, lowCutQ);
                
                updateCoefficients (leftLowCut.coefficients, newCoefficients);
                updateCoefficients (rightLowCut.coefficients, newCoefficients);
                break;
        }
    }
    else if (parameterID.startsWith ("Q"))
    {
        auto lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                auto& leftLowCut = leftChain.get<ChainPosition::lowCut>();
                auto& rightLowCut = rightChain.get<ChainPosition::lowCut>();
                
                lowCutQ = newValue;
                auto newCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), lowCutFreq, lowCutQ);
                
                updateCoefficients (leftLowCut.coefficients, newCoefficients);
                updateCoefficients (rightLowCut.coefficients, newCoefficients);
                break;
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // 添加全局音频参数
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"Scale", 1},
                                                             "Scale",
                                                             juce::NormalisableRange<float> (0.f, 150.f, 1.f),
                                                             100.f));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {"Gain", 1},
                                                             "Gain",
                                                             juce::NormalisableRange<float> (-12.f, 12.f, 0.1f),
                                                             0.f));
    
    // 添加每个滤波器的参数
    for (int i = 1; i <= 6; ++i)
    {
        String bypassString ("Bypass");
        bypassString << i;
        
        layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {bypassString, 1},
                                                                bypassString,
                                                                true));
        
        String typeString ("Type");
        typeString << i;
        
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {typeString, 1},
                                                                typeString,
                                                                StringArray ("Low Cut", "High Cut", "Bell", "Notch", "band Pass"),
                                                                1));
        
        String freqString ("Freq");
        freqString << i;
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                 freqString,
                                                                 juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                 100.f));
        
        String gainString ("Gain");
        gainString << i;
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {gainString, 1},
                                                                 gainString,
                                                                 juce::NormalisableRange<float> (-12.f, 12.f, 0.1f),
                                                                 0.f));
        
        String QString ("Q");
        QString << i;
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {QString, 1},
                                                                 QString,
                                                                 juce::NormalisableRange<float> (0.1f, 10.f, 0.1f),
                                                                 1.f));
    }
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
