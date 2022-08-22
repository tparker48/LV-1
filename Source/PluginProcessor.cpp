#include "PluginProcessor.h"
#include "PluginEditor.h"

LV1AudioProcessor::LV1AudioProcessor()
     : AudioProcessor (BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
       vts(*this, nullptr, "parameters", createParameterLayout())
{

    for (int i = 0; i < 12; i++)
    {
        synth.addSound(new SynthSound());
        synth.addVoice(new SynthVoice());
    }
}

LV1AudioProcessor::~LV1AudioProcessor() {}
void LV1AudioProcessor::setCurrentProgram (int index) {}
const juce::String LV1AudioProcessor::getProgramName (int index) { return {}; }
void LV1AudioProcessor::changeProgramName (int index, const juce::String& newName) {}


void LV1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getNumOutputChannels());
        }
    }
}

void LV1AudioProcessor::releaseResources() {}

bool LV1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}


void LV1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto tremoloAmt = vts.getRawParameterValue("bigKnob");
            auto tremoloHz = vts.getRawParameterValue("mediumKnob");
            auto attack = vts.getRawParameterValue("smallKnob1");
            auto sK2 = vts.getRawParameterValue("smallKnob2");
            auto filterCutoff = vts.getRawParameterValue("smallKnob3");
            auto noiseLevel = vts.getRawParameterValue("slider");
            auto crunchLevel= vts.getRawParameterValue("toggle");

            voice->setTremolo(*tremoloAmt, *tremoloHz);
            voice->setNoiseLevel(*noiseLevel);
            voice->setCrunchLevel(*crunchLevel);
            voice->setFilterCutoff(*filterCutoff);
            voice->setAttack(*attack);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void LV1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LV1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(vts.state.getType()))
            vts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

AudioProcessorValueTreeState::ParameterLayout LV1AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    auto bigKnob = std::make_unique<AudioParameterFloat>("bigKnob", "bK",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f), 0.5f);

    auto mediumKnob = std::make_unique<AudioParameterFloat>("mediumKnob", "mK",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f), 0.4f);

    auto smallKnob1 = std::make_unique<AudioParameterFloat>("smallKnob1", "sK1",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f), 0.33f);

    auto smallKnob2 = std::make_unique<AudioParameterFloat>("smallKnob2", "sK2",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f), 0.33f);

    auto smallKnob3 = std::make_unique<AudioParameterFloat>("smallKnob3", "sK3",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.3f), 0.75f);

    auto slider = std::make_unique<AudioParameterFloat>("slider", "s",
        NormalisableRange<float>(0.0f, 1.0f, 0.0f), 0.3f);

    auto toggle = std::make_unique<AudioParameterFloat>("toggle", "t", 1.0, 3.0, 3.0);

    params.push_back(std::move(bigKnob));
    params.push_back(std::move(mediumKnob));
    params.push_back(std::move(smallKnob1));
    params.push_back(std::move(smallKnob2));
    params.push_back(std::move(smallKnob3));
    params.push_back(std::move(slider));
    params.push_back(std::move(toggle));

    return { params.begin(), params.end() };
}

juce::AudioProcessorEditor* LV1AudioProcessor::createEditor() { return new LV1AudioProcessorEditor(*this, vts); };
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new LV1AudioProcessor(); }