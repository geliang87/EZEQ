/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetPanel.h"

//==============================================================================
class ResponseCurveComponent: public juce::Component,
                                     juce::AudioProcessorParameter::Listener,
                                     juce::Timer
{
public:
    ResponseCurveComponent (SimpleEQAudioProcessor&);
    ~ResponseCurveComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;
    
    void timerCallback() override;
    
    void updateResponseCurve();
    Path responseCurve;
    
private:
    SimpleEQAudioProcessor& audioProcessor;
    
    juce::Atomic<bool> parametersChanged { false };
    
    // 频率轴
    Array<float> frequencies { 20, 50, 100,
                               200, 500, 1000,
                               2000, 5000, 10000,
                               20000 };
    
    // 幅度轴
    Array<float> gains { -24, -12, 0, 12, 24 };
    
    void drawBackgroundGrid (juce::Graphics& g);
    void drawTextLabels (juce::Graphics& g);
};

//==============================================================================
/**
*/
class SimpleEQLookAndFeel : public LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(juce::Colours::darkorange);
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::lightyellow);
        g.drawEllipse(rx, ry, rw, rw, 3.0f);

        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::lightyellow);
        g.fillPath(p);
    }

    
    void drawButtonBackground(Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

        auto baseColour = juce::Colours::darkorange.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour(baseColour);

        auto flatOnLeft = button.isConnectedOnLeft();
        auto flatOnRight = button.isConnectedOnRight();
        auto flatOnTop = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            Path path;
            path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                bounds.getWidth(), bounds.getHeight(),
                cornerSize, cornerSize,
                !(flatOnLeft || flatOnTop),
                !(flatOnRight || flatOnTop),
                !(flatOnLeft || flatOnBottom),
                !(flatOnRight || flatOnBottom));

            g.fillPath(path);

            g.setColour(button.findColour(ComboBox::outlineColourId));
            g.strokePath(path, PathStrokeType(1.0f));
        }
        else
        {
            g.fillRoundedRectangle(bounds, cornerSize);

            g.setColour(Colours::lightyellow);
            g.drawRoundedRectangle(bounds, cornerSize, 3.0f);
        }
    }

};

//==============================================================================
class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor,
public Timer
{
public:
    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    int selectedFilter = 1;
    void mouseDown (const MouseEvent& event) override;
    
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

    SimpleEQLookAndFeel eqLNF;

    Image background;

    Gui::PresetPanel presetPannel;
    
    ResponseCurveComponent responseCurveComponent;

    juce::Slider freqSlider, freqGainSlider, qualitySlider, scaleSlider, gainSlider;
    juce::TextButton analysisButton;
    OwnedArray<juce::TextButton> freqButtons;
    OwnedArray<juce::ComboBox> typeCombos;
    juce::Label freqText, freqGainText, quailtyText, scaleText, gainText;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    std::unique_ptr<Attachment> scaleSliderAttachment,gainSliderAttachment,
    freqSliderAttachment, freqGainSliderAttachment, qualitySliderAttachment;
    
    OwnedArray<APVTS::ComboBoxAttachment> typeComboBoxAttachments;
    OwnedArray<APVTS::ButtonAttachment> freqButttonAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
