#pragma once

#include <algorithm>
#include "../JuceLibraryCode/JuceHeader.h"

struct SynthSound : public SynthesiserSound
{
    SynthSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class Flute : public SynthesiserVoice
{
public:

    Flute() { vts = NULL; }
    Flute(AudioProcessorValueTreeState* vts) { this->vts = vts; }

    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        SynthesiserSound* sound, int /*currentPitchWheelPosition*/) override;

    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    AudioProcessorValueTreeState* vts;

    double triangleWave(double phase);

    double hz;
    double phase, phase_delta;

    bool noteOn;
};
