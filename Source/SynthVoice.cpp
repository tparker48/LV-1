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
	crunchLevels = new float[3] { 60.0f, 10.0f, 1.0f };

	dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	spec.numChannels = numOutputChannels;

	gain.prepare(spec);
	gain.setGainLinear(0.2f);

	lpf.prepare(spec);
	lpf.setMode(dsp::LadderFilterMode::LPF24);
	lpf.setResonance(0.0f);

	osc.prepare(sampleRate);

	adsr.setSampleRate(sampleRate);
	adsr_params.decay = 0.0f;
	adsr_params.sustain = 1.0f;
	adsr_params.release = 0.16f;

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

	osc.setTremoloAmount(tremoloAmt);
	osc.setTremoloFrequency(tremoloHz);
	lpf.setCutoffFrequencyHz(filterCutoff);
	lpf.setDrive(crunch);

	dsp::AudioBlock<float> audioBlock(synthBuffer);

	osc.processBlock(synthBuffer, 0, synthBuffer.getNumSamples(), 1.0f - noiseAmt);
	noise.processBlock(synthBuffer, 0, synthBuffer.getNumSamples(), noiseAmt);
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


void SynthVoice::setTremolo(float level, float hz)
{
	tremoloAmt = level * 0.007f;
	tremoloHz = hz * 10.0f;
}


void SynthVoice::setNoiseLevel(float level)
{
	noiseAmt = level * 0.1;
}


void SynthVoice::setFilterCutoff(float cutoff)
{
	filterCutoff = 100.0f + cutoff * 19000.0f;
}


void SynthVoice::setCrunchLevel(float level)
{
	crunch = crunchLevels[int(level)-1];
}


void SynthVoice::setAttack(float amount)
{
	adsr_params.attack = 0.025f  + (amount * 0.7f);
	adsr.setParameters(adsr_params);
}