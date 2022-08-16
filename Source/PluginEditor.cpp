#include "PluginProcessor.h"
#include "PluginEditor.h"

LV1AudioProcessorEditor::LV1AudioProcessorEditor(LV1AudioProcessor& p, AudioProcessorValueTreeState& valTreeState)
    : AudioProcessorEditor(&p), audioProcessor(p), vts(valTreeState)
{
    this->addMouseListener(new MouseListener, false);

    double ratio = 7.0 / 3.0;
    setResizeLimits(400, 400 / ratio, 1600, 1600 / ratio);
    getConstrainer()->setFixedAspectRatio(ratio);
    setSize(1000.0, 1000.0 / ratio);
    
    dark = Colour(35, 35, 35);
    light = Colour(230, 230, 230);
    initLookAndFeels();

    initializeKnob(bigKnob, bigKnobP, "bigKnob");
    initializeKnob(mediumKnob, mediumKnobP, "mediumKnob");
    initializeKnob(smallKnob1, smallKnob1P, "smallKnob1");
    initializeKnob(smallKnob2, smallKnob2P, "smallKnob2");
    initializeKnob(smallKnob3, smallKnob3P, "smallKnob3");
    initializeSlider(slider, sliderP, "slider");
    initializeToggle(toggleState, toggleStateP, "toggle");
    initializeKnobSensitivities();

    resized();
}

LV1AudioProcessorEditor::~LV1AudioProcessorEditor()
{
    bigKnob.setLookAndFeel(nullptr);
    mediumKnob.setLookAndFeel(nullptr);
    smallKnob1.setLookAndFeel(nullptr);
    smallKnob2.setLookAndFeel(nullptr);
    smallKnob3.setLookAndFeel(nullptr);
    slider.setLookAndFeel(nullptr);

    delete customDial;
    delete customSlider;
    delete customButton;
}


void LV1AudioProcessorEditor::paint(Graphics& g)
{
    int borderW = getWidth() * 0.02;
    int borderH = getHeight() * 0.03;
    Image background = ImageCache::getFromMemory(BinaryData::ui_sketch_bg_dot_png, BinaryData::ui_sketch_bg_dot_pngSize);
    Rectangle<int> bounds = Rectangle<int>(borderW, borderH, getWidth() - 2 * borderW, getHeight() - 2 * borderH);

    g.fillAll(dark);
    g.drawImage(background, bounds.toFloat());
}


void LV1AudioProcessorEditor::resized()
{
    double w = getWidth();
    double h = getHeight();

    int largeDialSize = w * 0.20;
    int mediumDialSize = w * 0.1;
    int smallDialSize = w * 0.075;
    int sliderW = h * 0.63;
    int sliderH = w * 0.05;
    int buttonSize = w * 0.045;

    toggle1.setBounds(w * 0.06, h * 0.1, buttonSize, buttonSize);
    toggle2.setBounds(w * 0.06, h * 0.25, buttonSize, buttonSize);
    toggle3.setBounds(w * 0.06, h * 0.40, buttonSize, buttonSize);

    bigKnob.setBounds(w * 0.325, h * 0.375, largeDialSize, largeDialSize);
    mediumKnob.setBounds(w * 0.26, h * 0.17, mediumDialSize, mediumDialSize);
    smallKnob1.setBounds(w * 0.65, h * 0.1, smallDialSize, smallDialSize);
    smallKnob2.setBounds(w * 0.75, h * 0.1, smallDialSize, smallDialSize);
    smallKnob3.setBounds(w * 0.85, h * 0.1, smallDialSize, smallDialSize);

    slider.setBounds(w * 0.66, h * .4, sliderW, sliderH);   
}


void LV1AudioProcessorEditor::updateToggleState(int mode)
{
    toggleState.setSelectedId(mode);
}


void LV1AudioProcessorEditor::buttonStateChanged()
{
    int active = toggleState.getSelectedId();
    toggle1.setToggleState(active == 3, NotificationType::dontSendNotification);
    toggle2.setToggleState(active == 2, NotificationType::dontSendNotification);
    toggle3.setToggleState(active == 1, NotificationType::dontSendNotification);
}


void LV1AudioProcessorEditor::initLookAndFeels()
{
    getLookAndFeel().setColour(PopupMenu::ColourIds::backgroundColourId, dark);
    getLookAndFeel().setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, dark.darker(0.95));
    getLookAndFeel().setColour(PopupMenu::ColourIds::textColourId, light);
    getLookAndFeel().setColour(PopupMenu::ColourIds::highlightedTextColourId, light);

    customDial = new DialLookAndFeel(dark, light);
    customSlider = new SliderLookAndFeel(dark, light);
    customButton = new ButtonLookAndFeel(dark, light);
}


void LV1AudioProcessorEditor::updateLookAndFeels()
{
    getLookAndFeel().setColour(PopupMenu::ColourIds::backgroundColourId, dark);
    getLookAndFeel().setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, dark.darker(0.95));
    getLookAndFeel().setColour(PopupMenu::ColourIds::textColourId, light);
    getLookAndFeel().setColour(PopupMenu::ColourIds::highlightedTextColourId, light);

    customDial->setColors(dark, light);
    customSlider->setColors(dark, light);
    customButton->setColors(dark, light);
}


void LV1AudioProcessorEditor::initializeKnob(Slider& obj, std::unique_ptr<SliderAttachment>& objP, std::string name)
{
    addAndMakeVisible(obj);
    obj.setVelocityBasedMode(true);
    obj.setSliderStyle(Slider::LinearBarVertical);
    objP.reset(new SliderAttachment(vts, name, obj));
    obj.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    obj.setPopupDisplayEnabled(false, false, this);
    obj.setLookAndFeel(customDial);
}


void LV1AudioProcessorEditor::initializeSlider(Slider& obj, std::unique_ptr<SliderAttachment>& objP, std::string name)
{
    addAndMakeVisible(obj);
    objP.reset(new SliderAttachment(vts, name, obj));
    obj.setSliderStyle(Slider::LinearHorizontal);
    obj.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    obj.setPopupDisplayEnabled(false, false, this);
    obj.setLookAndFeel(customSlider);
}


void LV1AudioProcessorEditor::initializeToggle(ComboBox &obj, std::unique_ptr<ComboBoxAttachment>& objP, std::string name)
{
    addAndMakeVisible(obj);
    obj.addItem("small", 1);
    obj.addItem("medium", 2);
    obj.addItem("full", 3);
    obj.onChange = [this] { buttonStateChanged(); };
    objP.reset(new ComboBoxAttachment(vts, name, obj));

    initializeButton(toggle1, 3);
    initializeButton(toggle2, 2);
    initializeButton(toggle3, 1);
}


void LV1AudioProcessorEditor::initializeButton(ToggleButton&obj, int toggleId)
{
    addAndMakeVisible(obj);
    obj.onClick = [this, toggleId] { updateToggleState(toggleId); };
    obj.setRadioGroupId(1001);
    obj.setClickingTogglesState(true);
    obj.setLookAndFeel(customButton);
}


void LV1AudioProcessorEditor::initializeKnobSensitivities()
{
    float smallKnobSensitivity = 0.03;
    float mediumKnobSensitivity = 0.04;
    float bigKnobSensitivity = 0.045;

    smallKnob1.setVelocityModeParameters(1.0, 0, smallKnobSensitivity, false);
    smallKnob2.setVelocityModeParameters(1.0, 0, smallKnobSensitivity, false);
    smallKnob3.setVelocityModeParameters(1.0, 0, smallKnobSensitivity, false);
    mediumKnob.setVelocityModeParameters(1.0, 0, mediumKnobSensitivity, false);
    bigKnob.setVelocityModeParameters(1.0, 0, bigKnobSensitivity, false);
}