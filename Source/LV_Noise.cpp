#include "LV_Noise.h"


void LV_Noise::processBlock(AudioSampleBuffer& buffer, int startSample, int numSamples, float gain)
{
	for (int sample = 0; sample < numSamples; sample++)
	{
		for (int channel = 0; channel < buffer.getNumChannels(); channel++)
		{
			buffer.addSample(channel, sample, gain * noise());
		}
	}
}


float LV_Noise::noise()
{
	return random_generator.nextFloat();
}