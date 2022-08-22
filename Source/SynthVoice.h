#pragma once

#include "JuceHeader.h"
#include "SynthSound.h"
#include "LV_Oscillator.h"
#include "LV_Noise.h"

class SynthVoice : public SynthesiserVoice
{
public:
	bool canPlaySound(SynthesiserSound*) override;
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound * sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels);
	void renderNextBlock(AudioBuffer< float > & outputBuffer, int startSample, int numSamples) override;

	float tremoloAmt, tremoloHz;
	float noiseAmt;
	float filterCutoff;
	float crunchLevel;

private:
	LV_Oscillator osc;
	LV_Noise noise;

	dsp::Gain<float> gain;
	dsp::LadderFilter<float> lpf;
	ADSR adsr;
	ADSR::Parameters adsr_params;
	AudioBuffer<float> synthBuffer;

	float crunch;

	bool isPrepared;
};