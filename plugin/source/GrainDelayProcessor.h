#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "CircularAudioBuffer.h"

namespace audio_plugin {

class GrainDelayProcessor
{
public:

  static constexpr float kSpreadMsMax = 500.f;
  static constexpr float kSpreadMsMin = 0.f;
  static constexpr float kSpreadMsDefault = 0.f;

  static constexpr float kPitchMax = 12.f;
  static constexpr float kPitchMin = -12.f;
  static constexpr float kPitchDefault = 0.f;

  static constexpr float kPitchRandMax = 12.f;
  static constexpr float kPitchRandMin = -12.f;
  static constexpr float kPitchRandDefault = 0.f;

  static constexpr float kGrainFreqHzMax = 500.f;
  static constexpr float kGrainFreqHzMin = 1.f;
  static constexpr float kGrainFreqHzDefault = 1.f;

  GrainDelayProcessor(
        juce::AudioParameterFloat &spreadMs,
        juce::AudioParameterFloat &pitch,
        juce::AudioParameterFloat &pitchRand,
        juce::AudioParameterFloat &grainFreqHz,
        juce::AudioParameterFloat &mix):
        spreadMs_(spreadMs),
        pitch_(pitch),
        pitchRand_(pitchRand),
        grainFreqHz_(grainFreqHz),
        mix_(mix)
    {
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
      delayBuffer_ = CircularAudioBuffer(static_cast<int>(spec.numChannels), static_cast<int>(spec.sampleRate * kSpreadMsMax / 1000.f));
      delayBuffer_.clear();
      delayWriteIdx_ = 0;
    }

    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept {
        juce::ignoreUnused(context);
        jassertfalse;
    }

    template <>
    void process (const juce::dsp::ProcessContextReplacing<float>& context) noexcept {
        auto input_block = context.getInputBlock();
        auto output_block = context.getOutputBlock();
        int next_write_idx = delayWriteIdx_;
        for (size_t channel = 0; channel < input_block.getNumChannels(); ++channel)
        {
            auto* input_samples  = input_block.getChannelPointer (channel);
            auto* output_samples = output_block.getChannelPointer(channel);

            // Write in new 'delayed' content.
            next_write_idx = delayBuffer_.copyFrom(channel, delayWriteIdx_, input_samples, input_block.getNumSamples());
            for (int i = 0; i < input_block.getNumSamples(); i++) {
              output_samples[i] = input_samples[i] + delayBuffer_.getSample(channel, next_write_idx + i);
            }
            // TODO: Process grains on new buffer (each active grain envelop should be summed)
            // For now let's just act as a 500ms delay.
        }
        delayWriteIdx_ = next_write_idx;
    }

    void reset() noexcept
    {
    }

private:
    CircularAudioBuffer delayBuffer_;
    int delayWriteIdx_;

    // Parameters
    juce::AudioParameterFloat &spreadMs_;
    juce::AudioParameterFloat &pitch_;
    juce::AudioParameterFloat &pitchRand_;
    juce::AudioParameterFloat &grainFreqHz_;
    juce::AudioParameterFloat &mix_;
};
}// namespace audio_plugin