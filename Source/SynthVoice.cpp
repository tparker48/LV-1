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
	// osc.setTremoloAmount(0.003);
	// osc.setTremoloFrequency(7.0);

	gain.prepare(spec);
	gain.setGainLinear(0.1f);

	lpf.prepare(spec);
	lpf.setMode(dsp::LadderFilterMode::LPF24);

	adsr_params.attack = 0.044f;
	adsr_params.decay = 0.0f;
	adsr_params.sustain = 1.0f;
	adsr_params.release = 0.16f;

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

	osc.setTremoloAmount(tremoloAmt * 0.007f);
	osc.setTremoloFrequency(tremoloHz * 10.0f);
	osc.processBlock(synthBuffer, 0, synthBuffer.getNumSamples(), 1.0f - (0.05*noiseAmt));
	noise.processBlock(synthBuffer, 0, synthBuffer.getNumSamples(), (0.05*noiseAmt));

	lpf.setCutoffFrequencyHz(75.0f + filterCutoff*18925.0f);
	lpf.setResonance(0.0f);

	if (crunchLevel == 3.0f)
	{
		crunch = 1.0f;
	}
	else if (crunchLevel == 2.0f)
	{
		crunch = 10.0f;
	}
	else
	{
		crunch = 60.0f;
	}

	lpf.setDrive(crunch);

	dsp::AudioBlock<float> audioBlock(synthBuffer);
	lpf.process(dsp::ProcessContextReplacing<float>(audioBlock));
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