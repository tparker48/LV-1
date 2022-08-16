#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "UI/SliderLookAndFeel.h"
#include "UI/DialLookAndFeel.h"
#include "UI/ButtonLookAndFeel.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class LV1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    LV1AudioProcessorEditor (LV1AudioProcessor& , AudioProcessorValueTreeState&);
    ~LV1AudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void initializeKnob(Slider& obj, std::unique_ptr<SliderAttachment>& objP, std::string name);
    void initializeSlider(Slider& obj, std::unique_ptr<SliderAttachment>& objP, std::string name);
    void initializeToggle(ComboBox& obj, std::unique_ptr<ComboBoxAttachment>& objP, std::string name);
    void initializeButton(ToggleButton& obj, int toggleId);
    void initializeKnobSensitivities();

    void initLookAndFeels();
    void updateLookAndFeels();

    void updateToggleState(int mode);
    void buttonStateChanged();

    LV1AudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& vts;
    
    Colour dark, light;

    Slider bigKnob, mediumKnob, smallKnob1, smallKnob2, smallKnob3;
    Slider slider;
    ComboBox toggleState;
    ToggleButton toggle1, toggle2, toggle3;

    std::unique_ptr<SliderAttachment> bigKnobP, mediumKnobP, smallKnob1P, smallKnob2P, smallKnob3P;
    std::unique_ptr<SliderAttachment> sliderP;
    std::unique_ptr<ComboBoxAttachment> toggleStateP;

    DialLookAndFeel* customDial;
    SliderLookAndFeel* customSlider;
    ButtonLookAndFeel* customButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LV1AudioProcessorEditor)
};
