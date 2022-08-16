/*
  ==============================================================================

    SliderLookAndFeel.cpp
    Created: 6 Sep 2020 10:05:30pm
    Author:  Tom

  ==============================================================================
*/

#include "SliderLookAndFeel.h"

void SliderLookAndFeel::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const Slider::SliderStyle, Slider&)
{
    int pos = (sliderPos - x) / width;

    g.setColour(dark);

    // bar
    int barHeight = height * 0.2;
    g.fillRect(x, y + height/2 - barHeight/2, width, barHeight);

    // thumb
    int thumbWidth = width * 0.075;
    g.fillRect((int)sliderPos - (thumbWidth /2), y, thumbWidth, height);

    g.setColour(light);

    // thumb detail
    int detailWidth = width * 0.02;
    int detailHeight = height * 0.8;
    g.fillRect((int)sliderPos - (detailWidth / 2), y + (height - detailHeight)/2, detailWidth, detailHeight);
}