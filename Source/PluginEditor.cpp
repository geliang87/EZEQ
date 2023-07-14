/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResponseCurveComponent::ResponseCurveComponent (SimpleEQAudioProcessor& p)
: audioProcessor (p)
{
    const auto& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->addListener(this);
    }
    
    startTimerHz (60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for( auto param : params )
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    drawBackgroundGrid (g);
    drawTextLabels (g);
}

void ResponseCurveComponent::resized()
{}

void ResponseCurveComponent::parameterValueChanged (int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void ResponseCurveComponent::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{}

void ResponseCurveComponent::timerCallback()
{
    if( parametersChanged.compareAndSetBool(false, true) )
        updateResponseCurve();
    
    repaint();
}

void ResponseCurveComponent::updateResponseCurve()
{}

void ResponseCurveComponent::drawBackgroundGrid (juce::Graphics& g)
{
    g.setColour (juce::Colours::black);
    g.fillAll();
    
    auto areaAnalysis = getLocalBounds();
    areaAnalysis.removeFromTop (16);
    areaAnalysis.removeFromBottom (6);
    areaAnalysis.removeFromLeft (20);
    areaAnalysis.removeFromRight (20);
    
    g.setColour (juce::Colours::white);
    
    // 绘制竖线
    for (auto f : frequencies)
    {
        auto normX = juce::mapFromLog10 (f, 20.f, 20000.f);
        g.drawVerticalLine (areaAnalysis.getX() + areaAnalysis.getWidth() * normX,
                            areaAnalysis.getY(),
                            areaAnalysis.getBottom());
    }
    
    // 绘制横线
    for (auto gDb : gains)
    {
        auto y = jmap (gDb, -24.f, 24.f, float (areaAnalysis.getBottom()), float (areaAnalysis.getY()));
        g.drawHorizontalLine (y,
                              areaAnalysis.getX(),
                              areaAnalysis.getRight());
    }
}

void ResponseCurveComponent::drawTextLabels (juce::Graphics& g)
{
    auto areaAnalysis = getLocalBounds();
    areaAnalysis.removeFromTop (16);
    areaAnalysis.removeFromBottom (6);
    areaAnalysis.removeFromLeft (20);
    areaAnalysis.removeFromRight (20);
    
    g.setFont (10);
    
    // 绘制频率字符
    for (auto f : frequencies)
    {
        auto normX = juce::mapFromLog10 (f, 20.f, 20000.f);
        
        bool addK = false;
        String str;
        if( f > 999.f )
        {
            addK = true;
            f /= 1000.f;
        }

        str << f;
        if( addK )
            str << "k";
        str << "Hz";
        
        Rectangle<int> r;
        r.setSize (g.getCurrentFont().getStringWidth(str), 10);
        r.setCentre(areaAnalysis.getX() + areaAnalysis.getWidth() * normX, 0);
        r.setY(1);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    // 绘制dB字符
    for (auto gDb : gains)
    {
        auto y = jmap (gDb, -24.f, 24.f, float (areaAnalysis.getBottom()), float (areaAnalysis.getY()));
        
        String str;
        if( gDb > 0 )
            str << "+";
        str << gDb;
        
        Rectangle<int> r;
        r.setSize (g.getCurrentFont().getStringWidth(str), 10);
        r.setX (getWidth() - g.getCurrentFont().getStringWidth(str));
        r.setCentre (r.getCentreX(), y);
        
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), presetPannel (p.getPresetManager()), responseCurveComponent (audioProcessor)
{
    setSize (800,500);
    startTimer (100);

    background = ImageCache::getFromMemory(BinaryData::TDMovieOut_0_png, BinaryData::TDMovieOut_0_pngSize);
    
    addAndMakeVisible(presetPannel);

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

    freqButtons.clear();
    for (int i = 0; i < 6; ++i)
    {
        auto button = new TextButton ();
        
        button->setClickingTogglesState(true);
        button->setButtonText(std::to_string (i + 1));
        button->setColour(TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
        button->setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
        button->setLookAndFeel(&eqLNF);
        addAndMakeVisible (button);
        
        freqButtons.add (button);
    }

    typeCombos.clear();
    for (int i = 0; i < 6; ++i)
    {
        auto combo = new ComboBox();

        combo->addItem("Low Cut", 1);
        combo->addItem("High Cut", 2);
        combo->addItem("Bell", 3);
        combo->addItem("Notch", 4);
        combo->addItem("Band Pass", 5);
        combo->setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
        combo->setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::lightyellow);
        combo->setColour(PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
        combo->setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);

        addAndMakeVisible(combo);

        typeCombos.add(combo);
    }

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

    
    for (int i = 0; i < 6; ++i)
    {
        String bypassString("Bypass");
        bypassString << i + 1;

        freqButttonAttachments.add(new APVTS::ButtonAttachment(audioProcessor.apvts, bypassString, *freqButtons[i]));
    }

    for (int i = 0; i < 6; ++i)
    {
        String typeString ("Type");
        typeString << i + 1;
        
        typeComboBoxAttachments.add (new APVTS::ComboBoxAttachment (audioProcessor.apvts, typeString, *typeCombos[i]));
    }
    
    // 配置曲线显示模块
    addAndMakeVisible (responseCurveComponent);
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
    presetPannel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1f)));
    
    auto width = getWidth();
    auto height = getHeight();

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

    if (freqButtons.size() == 6)
    {
        for (int i = 0; i < 6; ++i)
        {
            freqButtons[i]->setBounds(getLocalBounds().toFloat()
                                      .withTrimmedTop(380.0f / 500.0f * height)
                                      .withTrimmedLeft((125.0f + i * 95.0f) / 800.0f * width)
                                      .withWidth(80.0f / 800.0f * width)
                                      .withHeight(27.5f / 500.0f * height).toNearestInt());
        }
    }

    if (typeCombos.size() == 6)
    {
        for (int i = 0; i < 6; ++i)
        {
            typeCombos[i]->setBounds(getLocalBounds().toFloat()
                .withTrimmedTop(407.5f / 500.0f * height)
                .withTrimmedLeft((125.0f + i * 95.0f) / 800.0f * width)
                .withWidth(80.0f / 800.0f * width)
                .withHeight(27.5f / 500.0f * height).toNearestInt());
        }
    }

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
    
    responseCurveComponent.setBounds (getLocalBounds().toFloat()
                                      .withTrimmedTop (80.6f / 500.0f * height)
                                      .withTrimmedLeft (125.0f / 800.0f * width)
                                      .withWidth (555.0f / 800.0f * width)
                                      .withHeight(275.0f / 500.0f * height).toNearestInt());
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
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq4", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain4", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q4", qualitySlider));

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
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq5", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain5", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q5", qualitySlider));

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
        freqSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Freq6", freqSlider));

        freqGainSliderAttachment.reset(nullptr);
        freqGainSliderAttachment.reset(new Attachment(audioProcessor.apvts, "Gain6", freqGainSlider));

        qualitySliderAttachment.reset(nullptr);
        qualitySliderAttachment.reset(new Attachment(audioProcessor.apvts, "Q6", qualitySlider));

        selectedFilter = 6;
        repaint();
    }
}

void SimpleEQAudioProcessorEditor::timerCallback()
{
    // 判断初始化完成，进行绘制
    if (freqButtons.size() == 6)
        resized();
}
