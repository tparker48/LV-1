#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"

class LV1AudioProcessor  : public juce::AudioProcessor
{
public:
    LV1AudioProcessor();
    ~LV1AudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorEditor* createEditor() override;

    const juce::String LV1AudioProcessor::getName() const { return JucePlugin_Name; };
    bool hasEditor() const override { return true; };
    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return false; };
    bool isMidiEffect() const override { return false; };
    double getTailLengthSeconds() const override { return 0.0; };
    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };


private:
    Synthesiser synth;
    MidiKeyboardState keyboardState;
    AudioProcessorValueTreeState vts;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LV1AudioProcessor)
};