/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800,500);

    freqSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    freqSlider.setRange(10.0, 220000.0, 1.0);
    freqSlider.setDoubleClickReturnValue(true, 0.0f);
    freqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    freqSlider.setPopupDisplayEnabled(true, false, this);
    freqSlider.setTextValueSuffix(" Hz");

  
    freqGainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    freqGainSlider.setRange(-15.0, 15.0, 1.0);
    freqGainSlider.setDoubleClickReturnValue(true, 0.0f);
    freqGainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    freqGainSlider.setPopupDisplayEnabled(true, false, this);
    freqGainSlider.setTextValueSuffix(" dB");

    qualitySlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    qualitySlider.setRange(0.1, 18.0, 1.0);
    qualitySlider.setDoubleClickReturnValue(true, 0.0f);
    qualitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    qualitySlider.setPopupDisplayEnabled(true, false, this);
    

    scaleSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    scaleSlider.setRange(-200.0, 200.0, 1.0);
    scaleSlider.setDoubleClickReturnValue(true, 0.0f);
    scaleSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    scaleSlider.setPopupDisplayEnabled(true, false, this);
    scaleSlider.setTextValueSuffix(" %");

    gainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    gainSlider.setRange(-12.0, 12.0, 1.0);
    gainSlider.setDoubleClickReturnValue(true, 0.0f);
    gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    gainSlider.setPopupDisplayEnabled(true, false, this);
    gainSlider.setTextValueSuffix(" dB");

    analysisButton.setClickingTogglesState(true);
    analysisButton.setButtonText("analysis");

    analysisButton.setClickingTogglesState(true);
    analysisButton.setButtonText("analysis");

    freq1Button.setClickingTogglesState(true);
    freq1Button.setButtonText("1");

    freq2Button.setClickingTogglesState(true);
    freq2Button.setButtonText("2");

    freq3Button.setClickingTogglesState(true);
    freq3Button.setButtonText("3");

    freq4Button.setClickingTogglesState(true);
    freq4Button.setButtonText("4");

    freq5Button.setClickingTogglesState(true);
    freq5Button.setButtonText("5");

    freq6Button.setClickingTogglesState(true);
    freq6Button.setButtonText("6");

    freq1Combo.addItem("Low Cut",1);
    freq1Combo.addItem("High Cut", 2);
    freq1Combo.addItem("Bell", 3);
    freq1Combo.addItem("Notch", 4);
    freq1Combo.addItem("Band Pass", 5);

    freq2Combo.addItem("Low Cut", 1);
    freq2Combo.addItem("High Cut", 2);
    freq2Combo.addItem("Bell", 3);
    freq2Combo.addItem("Notch", 4);
    freq2Combo.addItem("Band Pass", 5);

    freq3Combo.addItem("Low Cut", 1);
    freq3Combo.addItem("High Cut", 2);
    freq3Combo.addItem("Bell", 3);
    freq3Combo.addItem("Notch", 4);
    freq3Combo.addItem("Band Pass", 5);

    freq4Combo.addItem("Low Cut", 1);
    freq4Combo.addItem("High Cut", 2);
    freq4Combo.addItem("Bell", 3);
    freq4Combo.addItem("Notch", 4);
    freq4Combo.addItem("Band Pass", 5);

    freq5Combo.addItem("Low Cut", 1);
    freq5Combo.addItem("High Cut", 2);
    freq5Combo.addItem("Bell", 3);
    freq5Combo.addItem("Notch", 4);
    freq5Combo.addItem("Band Pass", 5);

    freq6Combo.addItem("Low Cut", 1);
    freq6Combo.addItem("High Cut", 2);
    freq6Combo.addItem("Bell", 3);
    freq6Combo.addItem("Notch", 4);
    freq6Combo.addItem("Band Pass", 5);

    addAndMakeVisible(freqSlider);
    addAndMakeVisible(freqGainSlider);
    addAndMakeVisible(qualitySlider);
    addAndMakeVisible(scaleSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(analysisButton);
    addAndMakeVisible(freq1Button);
    addAndMakeVisible(freq2Button);
    addAndMakeVisible(freq3Button);
    addAndMakeVisible(freq4Button);
    addAndMakeVisible(freq5Button);
    addAndMakeVisible(freq6Button);
    addAndMakeVisible(freq1Combo);
    addAndMakeVisible(freq2Combo);
    addAndMakeVisible(freq3Combo);
    addAndMakeVisible(freq4Combo);
    addAndMakeVisible(freq5Combo);
    addAndMakeVisible(freq6Combo);
    
    scaleSliderAttachment.reset (new Attachment (audioProcessor.apvts, "Scale", scaleSlider));
    gainSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Gain", gainSlider));
    freqSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Freq1", freqSlider));
    freqGainSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Gain1", freqGainSlider));
    qualitySliderAttachment.reset(new Attachment (audioProcessor.apvts, "Q1", qualitySlider));
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint(juce::Graphics &g)
{
    auto width = getWidth();
    auto height = getHeight();
    
    g.setColour (Colours::red);
    
    if (selectedFilter == 1)
        g.drawRect(getLocalBounds().toFloat()
                   .withTrimmedTop (36.5f / 50.0f * height)
                   .withTrimmedLeft (12.0f / 80.0f * width)
                   .withWidth(9.0f / 80.0f * width)
                   .withHeight(9.0f / 50.0f * height).toNearestInt());
    else if (selectedFilter == 2)
        g.drawRect(getLocalBounds().toFloat()
                   .withTrimmedTop (36.5f / 50.0f * height)
                   .withTrimmedLeft (21.5f / 80.0f * width)
                   .withWidth(9.0f / 80.0f * width)
                   .withHeight(9.0f / 50.0f * height).toNearestInt());
}

void SimpleEQAudioProcessorEditor::resized()
{

    auto width = getWidth();
    auto height = getHeight();
//  responseCurveComponent.setBounds(getLocalBounds().toFloat()
//         .withTrimmedLeft (12.5f / 50.0f * height)
//        .withTrimmedTop (6.0f / 80.0f * width)
//        .withWidth (55.5f / 80.0f * width)
//        .withHeight (29.0f / 50.0f * height).toNearestInt());
    

   freqSlider.setBounds(getLocalBounds().toFloat()
        .withTrimmedTop(80.0f / 500.0f * height)
        .withTrimmedLeft(20.0f / 800.0f * width)
        .withWidth(80.0f / 800.0f * width)
        .withHeight(80.0f / 500.0f * height).toNearestInt());

   freqGainSlider.setBounds(getLocalBounds().toFloat()
       .withTrimmedTop(230.0f / 500.0f * height)
       .withTrimmedLeft(20.0f / 800.0f * width)
       .withWidth(80.0f / 800.0f * width)
       .withHeight(80.0f / 500.0f * height).toNearestInt());

   qualitySlider.setBounds(getLocalBounds().toFloat()
       .withTrimmedTop(370.0f / 500.0f * height)
       .withTrimmedLeft(20.0f / 800.0f * width)
       .withWidth(80.0f / 800.0f * width)
       .withHeight(80.0f / 500.0f * height).toNearestInt());

  scaleSlider.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(230.0f / 500.0f * height)
      .withTrimmedLeft(700.0f / 800.0f * width) 
      .withWidth(80.0f / 800.0f * width)
      .withHeight(80.0f / 500.0f * height).toNearestInt());

 gainSlider.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(370.0f / 500.0f * height)
      .withTrimmedLeft(700.0f / 800.0f * width)
      .withWidth(80.0f / 800.0f * width)
      .withHeight(80.0f / 500.0f * height).toNearestInt());

 //button

 analysisButton.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(80.0f / 500.0f * height)
     .withTrimmedLeft(700.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(80.0f / 500.0f * height).toNearestInt());

 freq1Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(125.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq2Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(220.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq3Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(315.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq4Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(410.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq5Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(505.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq6Button.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(380.0f / 500.0f * height)
     .withTrimmedLeft(600.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());
 
 //combobox

 freq1Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(125.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq2Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(220.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq3Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(315.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq4Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(410.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq5Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(505.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

 freq6Combo.setBounds(getLocalBounds().toFloat()
     .withTrimmedTop(407.5f / 500.0f * height)
     .withTrimmedLeft(600.0f / 800.0f * width)
     .withWidth(80.0f / 800.0f * width)
     .withHeight(27.5f / 500.0f * height).toNearestInt());

}

void SimpleEQAudioProcessorEditor::mouseDown (const MouseEvent& event)
{
    auto width = getWidth();
    auto height = getHeight();
    
    if (event.x >= 12.0f / 80.0f * width
        && event.x <= 21.0f / 80.0f * width
        && event.y >= 36.5f / 50.0f * height
        && event.y <= 45.5 / 50.0f * height
        && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Freq1", freqSlider));
        
        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Gain1", freqGainSlider));
        
        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment (audioProcessor.apvts, "Q1", qualitySlider));
        
        selectedFilter = 1;
        repaint();
    }
    else if (event.x >= 21.5f / 80.0f * width
             && event.x <= 30.5f / 80.0f * width
             && event.y >= 36.5f / 50.0f * height
             && event.y <= 45.5 / 50.0f * height
             && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Freq2", freqSlider));
        
        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment (audioProcessor.apvts, "Gain2", freqGainSlider));
        
        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment (audioProcessor.apvts, "Q2", qualitySlider));
        
        selectedFilter = 2;
        repaint();
    }
}
