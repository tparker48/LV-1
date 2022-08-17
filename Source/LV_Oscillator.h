#pragma once

#include "JuceHeader.h"

class LV_Oscillator
{
public:
	LV_Oscillator() { triWave = new float[5] {0.0f, 1.0f, 0.0f, -1.0f, 0.0f}; }

	~LV_Oscillator() {};

	void prepare(double sampleRate);

	void setFrequency(float frequency);

	void setTremoloAmount(float amount);
	void setTremoloFrequency(float frequency);

	void processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples);

private:
	float tri();
	void incrementPhase();

	float* triWave;

	double fs, hz, phase, delta;
	double tremolo_amt, tremolo_hz, tremolo_phase, tremolo_delta;
};