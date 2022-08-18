#pragma once

#include "JuceHeader.h"

class LV_Noise
{
public:
	LV_Noise()
	{
		random_generator.setSeedRandomly();
	}

	~LV_Noise() {};

	void processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples, float gain);

private:
	float noise();

	Random random_generator;
};