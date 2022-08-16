#include "Flute.h"

void Flute::startNote (int midiNoteNumber, float velocity,
                SynthesiserSound* sound, int /*currentPitchWheelPosition*/)
{
    hz = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    phase_delta = (2.0 * MathConstants<double>::pi * hz) / getSampleRate();
    phase = 0.0;

    noteOn = true;
}

void Flute::stopNote(float /*velocity*/, bool allowTailOff)
{
    noteOn = false;
}

void Flute::renderNextBlock(AudioBuffer<float>& outputBuffer,
    int startSample, int numSamples)
{
    for (int i = startSample; i < (startSample + numSamples); i++)
    {
        float gain = 0.0f;
        if (noteOn)
        {
            gain = 0.1f;
        }

        phase += phase_delta;
        if (phase > MathConstants<float>::twoPi) { phase -= MathConstants<float>::twoPi; }

        for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
        {
            outputBuffer.setSample(channel, i, gain*triangleWave(phase));
        }
    }     
}

double Flute::triangleWave(double phase)
{
    return sin(phase);

    float tri[] = {0.0f, 1.0f, 0.0, -1.0f};

    float idx_f = phase / (.5f * MathConstants<float>::pi);

    int lo = (int) floor(idx_f);
    int hi = (int) ceil(idx_f);
    float hi_amt = idx_f - (float)lo;
    float lo_amt = 1.0f - hi_amt;

    return tri[lo]*lo_amt + tri[hi]*hi_amt;
}