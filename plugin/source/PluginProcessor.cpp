#include "source/PluginProcessor.h"
#include "source/PluginEditor.h"

namespace audio_plugin {

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            /*parameterID=*/juce::ParameterID (PARAM_IDS[kSpreadMsId], /*versionHint=*/kSpreadMsId + 1),
            /*parameterName=*/PARAM_IDS[kSpreadMsId],
            /*minValue=*/GrainDelayProcessor::kSpreadMsMin,
            /*maxValue=*/GrainDelayProcessor::kSpreadMsMax,
            /*defaultValue=*/GrainDelayProcessor::kSpreadMsDefault));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            /*parameterID=*/juce::ParameterID (PARAM_IDS[kPitchId], /*versionHint=*/kPitchId + 1),
            /*parameterName=*/PARAM_IDS[kPitchId],
            /*minValue=*/GrainDelayProcessor::kPitchMin,
            /*maxValue=*/GrainDelayProcessor::kPitchMax,
            /*defaultValue=*/GrainDelayProcessor::kPitchDefault));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            /*parameterID=*/juce::ParameterID (PARAM_IDS[kPitchRandId], /*versionHint=*/kPitchRandId + 1),
            /*parameterName=*/PARAM_IDS[kPitchRandId],
            /*minValue=*/GrainDelayProcessor::kPitchRandMin,
            /*maxValue=*/GrainDelayProcessor::kPitchRandMax,
            /*defaultValue=*/GrainDelayProcessor::kPitchRandDefault));
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            /*parameterID=*/juce::ParameterID (PARAM_IDS[kGrainFreqHzId], /*versionHint=*/kGrainFreqHzId + 1),
            /*parameterName=*/PARAM_IDS[kGrainFreqHzId],
            /*minValue=*/GrainDelayProcessor::kGrainFreqHzMin,
            /*maxValue=*/GrainDelayProcessor::kGrainFreqHzMax,
            /*defaultValue=*/GrainDelayProcessor::kGrainFreqHzDefault));
        //layout.add (std::make_unique<juce::AudioParameterFloat> (
        //    /*parameterID=*/juce::ParameterID (PARAM_IDS[kMixId], /*versionHint=*/kMixId + 1),
        //    /*parameterName=*/PARAM_IDS[kMixId],
        //    /*minValue=*/GrainDelayProcessor::kMixMin,
        //    /*maxValue=*/GrainDelayProcessor::kMixMax,
        //    /*defaultValue=*/GrainDelayProcessor::kMixDefault));

        return layout;
    }

    PluginProcessor::PluginProcessor()
        : AudioProcessor(
              BusesProperties()
                  .withInput("Input", juce::AudioChannelSet::stereo(), true)
                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(
              /*processorToConnectTo=*/*this,
              /*undoManagerToUse=*/nullptr,
              /*valueTreeType=*/"PARAMETERS",
              createParameterLayout()),
          grainDelay_(
            *static_cast<juce::AudioParameterFloat*>(
                          apvts.getParameter(PARAM_IDS[kSpreadMsId])),
            *static_cast<juce::AudioParameterFloat*>(
                          apvts.getParameter(PARAM_IDS[kPitchId])),
                      *static_cast<juce::AudioParameterFloat*>(
                          apvts.getParameter(PARAM_IDS[kPitchRandId])),
                      *static_cast<juce::AudioParameterFloat*>(
                          apvts.getParameter(PARAM_IDS[kGrainFreqHzId])),
                      *static_cast<juce::AudioParameterFloat*>(
                          apvts.getParameter(PARAM_IDS[kMixId]))) {}

    PluginProcessor::~PluginProcessor() {}

    const juce::String PluginProcessor::getName() const {
      return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const {
  return false;
}

bool PluginProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int PluginProcessor::getCurrentProgram() { return 0; }

void PluginProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void PluginProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
  juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {

  auto spec = juce::dsp::ProcessSpec { .sampleRate = sampleRate, .maximumBlockSize = (uint) samplesPerBlock, .numChannels = 2 };
  grainDelay_.prepare (spec);
}

void PluginProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  auto block = juce::dsp::AudioBlock<float> (buffer);
  auto context = juce::dsp::ProcessContextReplacing<float> (block);
  grainDelay_.process (context);
}

bool PluginProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PluginProcessor::createEditor() {
  return new PluginEditor(*this);
}

void PluginProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void PluginProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}
} // namespace audio_plugin

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::PluginProcessor();
}
