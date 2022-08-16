#include "TriangleOsc.h"

void TriangleOsc::prepare(double sampleRate)
{
	fs = sampleRate;
	hz = 0.0;
	phase = 0.0;
	delta = 0.0;
}


void TriangleOsc::setFrequency(float frequency)
{
	hz = frequency;
	delta = (hz * MathConstants<double>::twoPi) / fs;
}


float TriangleOsc::getFrequency()
{
	return hz;
}


void TriangleOsc::processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples)
{
	for (int sample = 0; sample < numSamples; sample++)
	{
		for (int channel = 0; channel < buffer.getNumChannels(); channel++)
		{
			buffer.setSample(channel, sample, wave(phase));
		}
		incrementPhase();
	}
}


void TriangleOsc::incrementPhase()
{
	phase += delta;

	if (phase > MathConstants<double>::twoPi)
	{
		phase -= MathConstants<double>::twoPi;
	}
}


float TriangleOsc::wave(double phase)
{
	float idx = phase / MathConstants<float>::halfPi;
	
	int lo = floor(idx);
	int hi = ceil(idx);
	float hi_amt = idx - lo;
	float lo_amt = 1.0f - hi_amt;

	return (triWave[lo] * lo_amt) + (triWave[hi] * hi_amt);


}