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

    background = ImageCache::getFromMemory(BinaryData::TDMovieOut_0_png, BinaryData::TDMovieOut_0_pngSize);

    freqSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    freqSlider.setRange(10.0, 220000.0, 1.0);
    freqSlider.setDoubleClickReturnValue(true, 0.0f);
    freqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    freqSlider.setPopupDisplayEnabled(true, false, this);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.setLookAndFeel(&eqLNF);

  
    freqGainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    freqGainSlider.setRange(-15.0, 15.0, 1.0);
    freqGainSlider.setDoubleClickReturnValue(true, 0.0f);
    freqGainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    freqGainSlider.setPopupDisplayEnabled(true, false, this);
    freqGainSlider.setTextValueSuffix(" dB");
    freqGainSlider.setLookAndFeel(&eqLNF);

    qualitySlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    qualitySlider.setRange(0.1, 18.0, 1.0);
    qualitySlider.setDoubleClickReturnValue(true, 0.0f);
    qualitySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    qualitySlider.setPopupDisplayEnabled(true, false, this);
    qualitySlider.setLookAndFeel(&eqLNF);

    scaleSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    scaleSlider.setRange(-200.0, 200.0, 1.0);
    scaleSlider.setDoubleClickReturnValue(true, 0.0f);
    scaleSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    scaleSlider.setPopupDisplayEnabled(true, false, this);
    scaleSlider.setTextValueSuffix(" %");
    scaleSlider.setLookAndFeel(&eqLNF);

    gainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainSlider.setRange(-12.0, 12.0, 1.0);
    gainSlider.setDoubleClickReturnValue(true, 0.0f);
    gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 80, 20);
    gainSlider.setPopupDisplayEnabled(true, false, this);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setLookAndFeel(&eqLNF);

    analysisButton.setClickingTogglesState(true);
    analysisButton.setButtonText("analysis");
    analysisButton.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    analysisButton.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    analysisButton.setLookAndFeel(&eqLNF);


    //for (int i = 1; i <= 6; ++i)
    //{
    //    String freqString("freq");
    //    freqString << i<<;

    //    freqString;
    //    
    //}
    
    freq1Button.setClickingTogglesState(true);
    freq1Button.setButtonText("1");
    freq1Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq1Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq1Button.setLookAndFeel(&eqLNF);

    freq2Button.setClickingTogglesState(true);
    freq2Button.setButtonText("2");
    freq2Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq2Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq2Button.setLookAndFeel(&eqLNF);

    freq3Button.setClickingTogglesState(true);
    freq3Button.setButtonText("3");
    freq3Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq3Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq3Button.setLookAndFeel(&eqLNF);

    freq4Button.setClickingTogglesState(true);
    freq4Button.setButtonText("4");
    freq4Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq4Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq4Button.setLookAndFeel(&eqLNF);

    freq5Button.setClickingTogglesState(true);
    freq5Button.setButtonText("5");
    freq5Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq5Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq5Button.setLookAndFeel(&eqLNF);

    freq6Button.setClickingTogglesState(true);
    freq6Button.setButtonText("6");
    freq6Button.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
    freq6Button.setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    freq6Button.setLookAndFeel(&eqLNF);

    freq1Combo.addItem("Low Cut",1);
    freq1Combo.addItem("High Cut", 2);
    freq1Combo.addItem("Bell", 3);
    freq1Combo.addItem("Notch", 4);
    freq1Combo.addItem("Band Pass", 5);
    freq1Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq1Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq1Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq1Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);
    

    freq2Combo.addItem("Low Cut", 1);
    freq2Combo.addItem("High Cut", 2);
    freq2Combo.addItem("Bell", 3);
    freq2Combo.addItem("Notch", 4);
    freq2Combo.addItem("Band Pass", 5);
    freq2Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq2Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq2Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq2Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

    freq3Combo.addItem("Low Cut", 1);
    freq3Combo.addItem("High Cut", 2);
    freq3Combo.addItem("Bell", 3);
    freq3Combo.addItem("Notch", 4);
    freq3Combo.addItem("Band Pass", 5);
    freq3Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq3Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq3Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq3Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

    freq4Combo.addItem("Low Cut", 1);
    freq4Combo.addItem("High Cut", 2);
    freq4Combo.addItem("Bell", 3);
    freq4Combo.addItem("Notch", 4);
    freq4Combo.addItem("Band Pass", 5);
    freq4Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq4Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq4Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq4Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

    freq5Combo.addItem("Low Cut", 1);
    freq5Combo.addItem("High Cut", 2);
    freq5Combo.addItem("Bell", 3);
    freq5Combo.addItem("Notch", 4);
    freq5Combo.addItem("Band Pass", 5);
    freq5Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq5Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq5Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq5Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

    freq6Combo.addItem("Low Cut", 1);
    freq6Combo.addItem("High Cut", 2);
    freq6Combo.addItem("Bell", 3);
    freq6Combo.addItem("Notch", 4);
    freq6Combo.addItem("Band Pass", 5);
    freq6Combo.setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq6Combo.setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
    freq6Combo.setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
    freq6Combo.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

    freqText.setText("Freq", NotificationType::dontSendNotification);
    freqText.setFont(18.0f);
    freqText.setJustificationType(Justification::centred);
    freqText.setColour(Label::ColourIds::textColourId, Colours::lightyellow);

    freqGainText.setText("Gain", NotificationType::dontSendNotification);
    freqGainText.setFont(18.0f);
    freqGainText.setJustificationType(Justification::centred);
    freqGainText.setColour(Label::ColourIds::textColourId, Colours::lightyellow);

    quailtyText.setText("Q", NotificationType::dontSendNotification);
    quailtyText.setFont(18.0f);
    quailtyText.setJustificationType(Justification::centred);
    quailtyText.setColour(Label::ColourIds::textColourId, Colours::lightyellow);

    scaleText.setText("Scale", NotificationType::dontSendNotification);
    scaleText.setFont(18.0f);
    scaleText.setJustificationType(Justification::centred);
    scaleText.setColour(Label::ColourIds::textColourId, Colours::lightyellow);

    gainText.setText("Gain", NotificationType::dontSendNotification);
    gainText.setFont(18.0f);
    gainText.setJustificationType(Justification::centred);
    gainText.setColour(Label::ColourIds::textColourId, Colours::lightyellow);


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
    addAndMakeVisible(freqText);
    addAndMakeVisible(freqGainText);
    addAndMakeVisible(quailtyText);
    addAndMakeVisible(scaleText);
    addAndMakeVisible(gainText);
    
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

    g.drawImage(background,
        0, 0, 800, 500,
        0, 0, 1280, 720);

    auto width = getWidth();
    auto height = getHeight();
    
    g.setColour (Colours::lightyellow);
    
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

    else if (selectedFilter == 3)
        g.drawRect(getLocalBounds().toFloat()
            .withTrimmedTop(36.5f / 50.0f * height)
            .withTrimmedLeft(31.0f / 80.0f * width)
            .withWidth(9.0f / 80.0f * width)
            .withHeight(9.0f / 50.0f * height).toNearestInt());

    else if (selectedFilter == 4)
        g.drawRect(getLocalBounds().toFloat()
            .withTrimmedTop(36.5f / 50.0f * height)
            .withTrimmedLeft(40.5f / 80.0f * width)
            .withWidth(9.0f / 80.0f * width)
            .withHeight(9.0f / 50.0f * height).toNearestInt());

    else if (selectedFilter == 5)
        g.drawRect(getLocalBounds().toFloat()
            .withTrimmedTop(36.5f / 50.0f * height)
            .withTrimmedLeft(50.0f / 80.0f * width)
            .withWidth(9.0f / 80.0f * width)
            .withHeight(9.0f / 50.0f * height).toNearestInt());

    else if (selectedFilter == 6)
        g.drawRect(getLocalBounds().toFloat()
            .withTrimmedTop(36.5f / 50.0f * height)
            .withTrimmedLeft(59.5f / 80.0f * width)
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

 //label text

  freqText.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(55.0f / 500.0f * height)
      .withTrimmedLeft(20.0f / 800.0f * width)
      .withWidth(80.0f / 800.0f * width)
      .withHeight(32.5f / 500.0f * height).toNearestInt());

  freqGainText.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(205.0f / 500.0f * height)
      .withTrimmedLeft(20.0f / 800.0f * width)
      .withWidth(80.0f / 800.0f * width)
      .withHeight(32.5f / 500.0f * height).toNearestInt());

  quailtyText.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(345.0f / 500.0f * height)
      .withTrimmedLeft(20.0f / 800.0f * width) 
      .withWidth(80.0f / 800.0f * width)
      .withHeight(32.5f / 500.0f * height).toNearestInt());

  scaleText.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(205.0f / 500.0f * height)
      .withTrimmedLeft(700.0f / 800.0f * width)
      .withWidth(80.0f / 800.0f * width)
      .withHeight(32.5f / 500.0f * height).toNearestInt());

  gainText.setBounds(getLocalBounds().toFloat()
      .withTrimmedTop(345.0f / 500.0f * height)
      .withTrimmedLeft(700.0f / 800.0f * width)
      .withWidth(80.0f / 800.0f * width)
      .withHeight(32.5f / 500.0f * height).toNearestInt());

}

void SimpleEQAudioProcessorEditor::mouseDown(const MouseEvent& event)
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
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq1", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain1", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q1", qualitySlider));

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
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq2", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain2", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q2", qualitySlider));

        selectedFilter = 2;
        repaint();
    }

    else if (event.x >= 32.0f / 80.0f * width
        && event.x <= 40.0f / 80.0f * width
        && event.y >= 36.5f / 50.0f * height
        && event.y <= 45.5 / 50.0f * height
        && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq3", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain3", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q3", qualitySlider));

        selectedFilter = 3;
        repaint();
    }

    else if (event.x >= 41.5f / 80.0f * width
        && event.x <= 49.5f / 80.0f * width
        && event.y >= 36.5f / 50.0f * height
        && event.y <= 45.5 / 50.0f * height
        && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq3", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain3", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q3", qualitySlider));

        selectedFilter = 4;
        repaint();
    }

    else if (event.x >= 51.0f / 80.0f * width
        && event.x <= 59.0f / 80.0f * width
        && event.y >= 36.5f / 50.0f * height
        && event.y <= 45.5 / 50.0f * height
        && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq3", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain3", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q3", qualitySlider));

        selectedFilter = 5;
        repaint();
    }

    else if (event.x >= 60.5f / 80.0f * width
        && event.x <= 68.5 / 80.0f * width
        && event.y >= 36.5f / 50.0f * height
        && event.y <= 45.5 / 50.0f * height
        && event.mods.isLeftButtonDown())
    {
        freqSliderAttachment.reset(nullptr);
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq3", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain3", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q3", qualitySlider));

        selectedFilter = 6;
        repaint();
    }
}