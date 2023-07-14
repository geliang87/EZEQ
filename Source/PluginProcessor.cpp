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
    
    auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), lowCutFreq, lowCutQ);
    
    updateCoefficients (leftLowCut.coefficients, lowCutCoefficients);
    updateCoefficients (rightLowCut.coefficients, lowCutCoefficients);
    
    // filter2

    auto& leftFilter2 = leftChain.get<ChainPosition::filter2>();
    auto& rightFilter2 = rightChain.get<ChainPosition::filter2>();

    auto filter2Coefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter2Freq, filter2Q, filter2Gain);

    updateCoefficients(leftFilter2.coefficients, filter2Coefficients);
    updateCoefficients(rightFilter2.coefficients, filter2Coefficients);

    // filter3

    auto& leftFilter3 = leftChain.get<ChainPosition::filter3>();
    auto& rightFilter3 = rightChain.get<ChainPosition::filter3>();

    auto filter3Coefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter3Freq, filter3Q, filter3Gain);

    updateCoefficients(leftFilter3.coefficients, filter3Coefficients);
    updateCoefficients(rightFilter3.coefficients, filter3Coefficients);

    // filter4

    auto& leftFilter4 = leftChain.get<ChainPosition::filter4>();
    auto& rightFilter4 = rightChain.get<ChainPosition::filter4>();

    auto filter4Coefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter4Freq, filter4Q, filter4Gain);

    updateCoefficients(leftFilter4.coefficients, filter4Coefficients);
    updateCoefficients(rightFilter4.coefficients, filter4Coefficients);

    // filter5
    
    auto& leftFilter5 = leftChain.get<ChainPosition::filter5>();
    auto& rightFilter5 = rightChain.get<ChainPosition::filter5>();

    auto filter5Coefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter5Freq, filter5Q, filter5Gain);

    updateCoefficients(leftFilter5.coefficients, filter5Coefficients);
    updateCoefficients(rightFilter5.coefficients, filter5Coefficients);

    // highCut
    auto& leftHighCut = leftChain.get<ChainPosition::highCut>();
    auto& rightHighCut = rightChain.get<ChainPosition::highCut>();

    auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), highCutFreq, highCutQ);

    updateCoefficients(leftHighCut.coefficients, highCutCoefficients);
    updateCoefficients(rightHighCut.coefficients, highCutCoefficients);
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
        int lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                lowCutFreq = newValue;
                updateFilterSetup (1, FilterType::lowCutType, lowCutFreq, lowCutQ, 1.0f);
                break;
                
            case 2:
                filter2Freq = newValue;
                updateFilterSetup (2, filter2Type, filter2Freq, filter2Q, filter2Gain);
                break;
                
            case 3:
                filter3Freq = newValue;
                updateFilterSetup (3, filter3Type, filter3Freq, filter3Q, filter3Gain);
                break;
                
            case 4:
                filter4Freq = newValue;
                updateFilterSetup (4, filter4Type, filter4Freq, filter4Q, filter4Gain);
                break;
                
            case 5:
                filter5Freq = newValue;
                updateFilterSetup (5, filter5Type, filter5Freq, filter5Q, filter5Gain);
                break;
                
            case 6:
                highCutFreq = newValue;
                updateFilterSetup (6, FilterType::highCutType, highCutFreq, highCutQ, 1.0f);
                break;
        }
    }
    else if (parameterID.startsWith ("Q"))
    {
        int lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                lowCutQ = newValue;
                updateFilterSetup (1, FilterType::lowCutType, lowCutFreq, lowCutQ, 1.0f);
                break;
            
            case 2:
                filter2Q = newValue;
                updateFilterSetup (2, filter2Type, filter2Freq, filter2Q, filter2Gain);
                break;
                
            case 3:
                filter3Q = newValue;
                updateFilterSetup (3, filter3Type, filter3Freq, filter3Q, filter3Gain);
                break;
                
            case 4:
                filter4Q = newValue;
                updateFilterSetup (4, filter4Type, filter4Freq, filter4Q, filter4Gain);
                break;
                
            case 5:
                filter2Q = newValue;
                updateFilterSetup (5, filter5Type, filter5Freq, filter5Q, filter5Gain);
                break;
                
            case 6:
                highCutQ = newValue;
                updateFilterSetup (6, FilterType::highCutType, highCutFreq, highCutQ, 1.0f);
                break;
        }
    }
    else if (parameterID.startsWith ("Gain"))
    {
        int lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                updateFilterSetup (1, FilterType::lowCutType, lowCutFreq, lowCutQ, 1.0f);
                break;
            
            case 2:
                filter2Gain = Decibels::decibelsToGain (newValue);
                updateFilterSetup (2, filter2Type, filter2Freq, filter2Q, filter2Gain);
                break;
                
            case 3:
                filter3Gain = Decibels::decibelsToGain (newValue);
                updateFilterSetup (3, filter3Type, filter3Freq, filter3Q, filter3Gain);
                break;
                
            case 4:
                filter4Gain = Decibels::decibelsToGain (newValue);
                updateFilterSetup (4, filter4Type, filter4Freq, filter4Q, filter4Gain);
                break;
                
            case 5:
                filter5Gain = Decibels::decibelsToGain (newValue);
                updateFilterSetup (5, filter5Type, filter5Freq, filter5Q, filter5Gain);
                break;
                
            case 6:
                updateFilterSetup (6, FilterType::highCutType, highCutFreq, highCutQ, 1.0f);
                break;
        }
    }
    else if (parameterID.startsWith ("Type"))
    {
        int lastDigit = String::charToString (parameterID.getLastCharacter()).getIntValue();
        
        switch (lastDigit)
        {
            case 1:
                updateFilterSetup (1, FilterType::lowCutType, lowCutFreq, lowCutQ, 1.0f);
                break;
            
            case 2:
                filter2Type = static_cast<FilterType> (newValue);
                updateFilterSetup (2, filter2Type, filter2Freq, filter2Q, filter2Gain);
                break;
                
            case 3:
                filter3Gain = static_cast<FilterType> (newValue);
                updateFilterSetup (3, filter3Type, filter3Freq, filter3Q, filter3Gain);
                break;
                
            case 4:
                filter4Gain = static_cast<FilterType> (newValue);
                updateFilterSetup (4, filter4Type, filter4Freq, filter4Q, filter4Gain);
                break;
                
            case 5:
                filter5Gain = static_cast<FilterType> (newValue);
                updateFilterSetup (5, filter5Type, filter5Freq, filter5Q, filter5Gain);
                break;
                
            case 6:
                updateFilterSetup (6, FilterType::highCutType, highCutFreq, highCutQ, 1.0f);
                break;
        }
    }

    else if (parameterID.startsWith("Bypass"))
    {
        int lastDigit = String::charToString(parameterID.getLastCharacter()).getIntValue();
        bool flag = (newValue == 0.0f) ? false : true;

        switch (lastDigit)
        {
            case 1:
                leftChain.setBypassed<ChainPosition::lowCut>(flag);
                rightChain.setBypassed<ChainPosition::lowCut>(flag);
                break;

            case 2:
                leftChain.setBypassed<ChainPosition::filter2>(flag);
                rightChain.setBypassed<ChainPosition::filter2>(flag);
                break;

            case 3:
                leftChain.setBypassed<ChainPosition::filter3>(flag);
                rightChain.setBypassed<ChainPosition::filter3>(flag);
                break;

            case 4:
                leftChain.setBypassed<ChainPosition::filter4>(flag);
                rightChain.setBypassed<ChainPosition::filter4>(flag);
                break;

            case 5:
                leftChain.setBypassed<ChainPosition::filter5>(flag);
                rightChain.setBypassed<ChainPosition::filter5>(flag);
                break;

            case 6:
                leftChain.setBypassed<ChainPosition::highCut>(flag);
                rightChain.setBypassed<ChainPosition::highCut>(flag);
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
        
        if (i == 1)
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {typeString, 1},
                                                                      typeString,
                                                                      StringArray ("Low Cut", "High Cut", "Bell", "Notch", "Band Pass"),
                                                                      0));
        else if (i == 6)
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {typeString, 1},
                                                                      typeString,
                                                                      StringArray ("Low Cut", "High Cut", "Bell", "Notch", "Band Pass"),
                                                                      1));
        else
            layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID {typeString, 1},
                                                                      typeString,
                                                                      StringArray ("Low Cut", "High Cut", "Bell", "Notch", "Band Pass"),
                                                                      2));
        
        String freqString ("Freq");
        freqString << i;
        
        if (i == 1)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     100.f));
        else if (i == 2)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     200.f));
        else if (i == 3)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     400.f));
        else if (i == 4)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     800.f));
        else if (i == 5)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     1600.f));
        else if (i == 6)
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {freqString, 1},
                                                                     freqString,
                                                                     juce::NormalisableRange<float> (20.f, 20000.f, 1.f),
                                                                     20000.f));
        
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

void SimpleEQAudioProcessor::updateFilterSetup (int filterIndex, FilterType type, float freq, float Q, float gain)
{
    if (filterIndex == 1)
    {
        auto& filterLeft = leftChain.get<ChainPosition::lowCut>();
        auto& filterRight = rightChain.get<ChainPosition::lowCut>();
        
        auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), freq, Q);
        
        updateCoefficients (filterLeft.coefficients, lowCutCoefficients);
        updateCoefficients (filterRight.coefficients, lowCutCoefficients);
    }
    else if (filterIndex == 2)
    {
        auto& filterLeft = leftChain.get<ChainPosition::filter2>();
        auto& filterRight = rightChain.get<ChainPosition::filter2>();
        
        if (type == FilterType::lowCutType)
        {
            auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), filter2Freq, filter2Q);
            
            updateCoefficients (filterLeft.coefficients, lowCutCoefficients);
            updateCoefficients (filterRight.coefficients, lowCutCoefficients);
        }
        else if (type == FilterType::highCutType)
        {
            auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filter2Freq, filter2Q);

            updateCoefficients(filterLeft.coefficients, highCutCoefficients);
            updateCoefficients(filterRight.coefficients, highCutCoefficients);
        }
        else if (type == FilterType::bellType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter2Freq, filter2Q, filter2Gain);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::notchType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeNotch(getSampleRate(), filter2Freq, filter2Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::bandPassType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), filter2Freq, filter2Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
    }
    else if (filterIndex == 3)
    {
        auto& filterLeft = leftChain.get<ChainPosition::filter3>();
        auto& filterRight = rightChain.get<ChainPosition::filter3>();
        
        if (type == FilterType::lowCutType)
        {
            auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), filter3Freq, filter3Q);
            
            updateCoefficients (filterLeft.coefficients, lowCutCoefficients);
            updateCoefficients (filterRight.coefficients, lowCutCoefficients);
        }
        else if (type == FilterType::highCutType)
        {
            auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filter3Freq, filter3Q);

            updateCoefficients(filterLeft.coefficients, highCutCoefficients);
            updateCoefficients(filterRight.coefficients, highCutCoefficients);
        }
        else if (type == FilterType::bellType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter3Freq, filter3Q, filter3Gain);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::notchType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeNotch(getSampleRate(), filter3Freq, filter3Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::bandPassType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), filter3Freq, filter3Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
    }
    else if (filterIndex == 4)
    {
        auto& filterLeft = leftChain.get<ChainPosition::filter4>();
        auto& filterRight = rightChain.get<ChainPosition::filter4>();
        
        if (type == FilterType::lowCutType)
        {
            auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), filter4Freq, filter4Q);
            
            updateCoefficients (filterLeft.coefficients, lowCutCoefficients);
            updateCoefficients (filterRight.coefficients, lowCutCoefficients);
        }
        else if (type == FilterType::highCutType)
        {
            auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filter4Freq, filter4Q);

            updateCoefficients(filterLeft.coefficients, highCutCoefficients);
            updateCoefficients(filterRight.coefficients, highCutCoefficients);
        }
        else if (type == FilterType::bellType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter4Freq, filter4Q, filter4Gain);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::notchType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeNotch(getSampleRate(), filter4Freq, filter4Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::bandPassType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), filter4Freq, filter4Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
    }
    else if (filterIndex == 5)
    {
        auto& filterLeft = leftChain.get<ChainPosition::filter5>();
        auto& filterRight = rightChain.get<ChainPosition::filter5>();
        
        if (type == FilterType::lowCutType)
        {
            auto lowCutCoefficients = dsp::IIR::Coefficients<float>::makeHighPass (getSampleRate(), filter5Freq, filter5Q);
            
            updateCoefficients (filterLeft.coefficients, lowCutCoefficients);
            updateCoefficients (filterRight.coefficients, lowCutCoefficients);
        }
        else if (type == FilterType::highCutType)
        {
            auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filter5Freq, filter5Q);

            updateCoefficients(filterLeft.coefficients, highCutCoefficients);
            updateCoefficients(filterRight.coefficients, highCutCoefficients);
        }
        else if (type == FilterType::bellType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), filter5Freq, filter5Q, filter5Gain);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::notchType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeNotch(getSampleRate(), filter5Freq, filter5Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
        else if (type == FilterType::bandPassType)
        {
            auto newCoefficients = dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), filter5Freq, filter5Q);

            updateCoefficients(filterLeft.coefficients, newCoefficients);
            updateCoefficients(filterRight.coefficients, newCoefficients);
        }
    }
    else if (filterIndex == 6)
    {
        auto& filterLeft = leftChain.get<ChainPosition::highCut>();
        auto& filterRight = rightChain.get<ChainPosition::highCut>();
        
        auto highCutCoefficients = dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), highCutFreq, highCutQ);

        updateCoefficients(filterLeft.coefficients, highCutCoefficients);
        updateCoefficients(filterRight.coefficients, highCutCoefficients);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
