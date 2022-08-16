#pragma once

#include "JuceHeader.h"

class TriangleOsc
{
public:
	TriangleOsc() 
	{
		triWave = new float[5] {0.0f, 1.0f, 0.0f, -1.0f, 0.0f};
	}

	~TriangleOsc() {};

	void prepare(double sampleRate);

	void setFrequency(float frequency);
	float getFrequency();

	void processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples);

private:
	float wave(double phase);
	void incrementPhase();

	double fs, hz, phase, delta;

	float* triWave;
};