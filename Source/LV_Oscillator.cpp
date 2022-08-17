#include "LV_Oscillator.h"

void LV_Oscillator::prepare(double sampleRate)
{
	fs = sampleRate;
	
	hz = 0.0;
	phase = 0.0;
	delta = 0.0;

	tremolo_hz = 0.0;
	tremolo_phase = 0.0;
	tremolo_delta = 0.0;
}


void LV_Oscillator::setFrequency(float frequency)
{
	hz = frequency;
	delta = (hz * MathConstants<double>::twoPi) / fs;
}


void LV_Oscillator::setTremoloAmount(float amount)
{
	tremolo_amt = amount;
}


void LV_Oscillator::setTremoloFrequency(float frequency)
{
	tremolo_hz = frequency;
	tremolo_delta = (tremolo_hz * MathConstants<double>::twoPi) / fs;
}


void LV_Oscillator::processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples)
{
	for (int sample = 0; sample < numSamples; sample++)
	{
		for (int channel = 0; channel < buffer.getNumChannels(); channel++)
		{
			buffer.addSample(channel, sample, tri());
		}
		incrementPhase();
	}
}


void LV_Oscillator::incrementPhase()
{
	double real_delta = delta + (tremolo_amt * delta * sin(tremolo_phase));
	phase += real_delta;
	if (phase > MathConstants<double>::twoPi) 
	{ 
		phase -= MathConstants<double>::twoPi; 
	}

	tremolo_phase += tremolo_delta;
	if (tremolo_phase > MathConstants<double>::twoPi)
	{ 
		tremolo_phase -= MathConstants<double>::twoPi;
	}
}


float LV_Oscillator::tri()
{
	float idx = phase / MathConstants<float>::halfPi;
	
	int lo = floor(idx);
	int hi = ceil(idx);
	float hi_amt = idx - lo;
	float lo_amt = 1.0f - hi_amt;

	return (triWave[lo] * lo_amt) + (triWave[hi] * hi_amt);
}