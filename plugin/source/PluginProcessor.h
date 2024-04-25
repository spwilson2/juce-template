#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "GrainDelayProcessor.h"

namespace audio_plugin {
static constexpr std::array<const char*, 5> PARAM_IDS = { "Spread", "Pitch", "Pitch Rand", "Frequency", "Mix" };
enum ParameterIds {
    kSpreadMsId,
    kPitchId,
    kPitchRandId,
    kGrainFreqHzId,
    kMixId,
    LAST
};
static_assert(PARAM_IDS.size() == ParameterIds::LAST);

class PluginProcessor : public juce::AudioProcessor {
public:


  PluginProcessor();
  ~PluginProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState &getApvtsRef() {
      return this->apvts;
  }

private:        
  juce::AudioProcessorValueTreeState apvts;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)

  GrainDelayProcessor grainDelay_;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
} // namespace audio_plugin
