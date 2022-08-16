#include "SynthVoice.h"

bool SynthVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<SynthesiserSound*>(sound) != nullptr;
}


void SynthVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)  
{
	osc.setFrequency(MidiMessage::getMidiNoteInHertz(midiNoteNumber));
	adsr.noteOn();
}


void SynthVoice::stopNote(float velocity, bool allowTailOff)  
{
	adsr.noteOff();

	if (!allowTailOff || !adsr.isActive())
	{
		clearCurrentNote();
	}
}


void SynthVoice::pitchWheelMoved(int newPitchWheelValue)  
{}


void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)  
{}


void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels)
{
	adsr.setSampleRate(sampleRate);

	dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	spec.numChannels = numOutputChannels;

	osc.prepare(sampleRate);

	gain.prepare(spec);
	
	gain.setGainLinear(0.1f);

	adsr_params.attack = 0.04f;
	adsr_params.decay = 0.0f;
	adsr_params.sustain = 1.0f;
	adsr_params.release = 0.2f;

	adsr.setParameters(adsr_params);

	isPrepared = true;
}


void SynthVoice::renderNextBlock(AudioBuffer< float >& outputBuffer, int startSample, int numSamples)  
{
	jassert (isPrepared);

	if (!isVoiceActive())
	{
		return;
	}

	synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
	synthBuffer.clear();

	
	osc.processBlock(synthBuffer, 0, synthBuffer.getNumSamples());
	dsp::AudioBlock<float> audioBlock(synthBuffer);
	gain.process(dsp::ProcessContextReplacing<float>(audioBlock));
	adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

	for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
	{
		outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
		
		if (!adsr.isActive())
		{
			clearCurrentNote();
		}
	}
}