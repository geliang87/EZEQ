/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
*/
class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    int selectedFilter = 1;
    void mouseDown (const MouseEvent& event) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

    juce::Slider freqSlider, freqGainSlider, qualitySlider, scaleSlider, gainSlider;
    juce::TextButton analysisButton, freq1Button, freq2Button, freq3Button, freq4Button, freq5Button, freq6Button;
    juce::ComboBox freq1Combo, freq2Combo, freq3Combo, freq4Combo, freq5Combo, freq6Combo;

    /*ResponseCurveComponent responseCurveComponent;*/
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    std::unique_ptr<Attachment> scaleSliderAttachment,gainSliderAttachment,
    freqSliderAttachment, freqGainSliderAttachment, qualitySliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
