#pragma once
#include <juce_dsp/juce_dsp.h>

struct CircularAudioBufferReadPointer {
  const float * buffer;
  int size;
};

class CircularAudioBuffer {
  public:
  CircularAudioBuffer():
    delayBuffer_(),
    size_(0)
   { }

  CircularAudioBuffer(int numChannelsToAllocate, int numSamplesToAllocate):
    delayBuffer_(numChannelsToAllocate, numSamplesToAllocate),
    size_(numSamplesToAllocate)
   {
  }

  float getSample(int channel, int sampleIndex) const {
    return delayBuffer_.getSample(channel, sampleIndex);
  }

  void getSplitReadPointers(int channel, int sampleIndex, CircularAudioBufferReadPointer pointers[2]) {
    // We should never attempt to access more than the buffer or start outside
    // the buffer, but accessing over the edge is handled.
    jassert(sampleIndex <= size_);
    pointers[0].buffer = delayBuffer_.getReadPointer(channel, sampleIndex);
    pointers[0].size = size_ - sampleIndex;
    pointers[1].buffer = delayBuffer_.getReadPointer(channel);
    pointers[1].size = sampleIndex;
  }

  int copyFrom(int destChannel, int destStartSample, const float *source, int numSamples) {

    // We should never attempt to write more than the buffer or start outside
    // the buffer, but writing over the edge is handled.
    jassert(numSamples <= size_);
    jassert(destStartSample < size_);

    int logical_end_sample = destStartSample + numSamples;
    int samples_to_write = numSamples;

    // If we would write over the end, write up until the end. Then copy the remainder to the start
    if (logical_end_sample >= size_) {
      samples_to_write = size_ - destStartSample;
      delayBuffer_.copyFrom(destChannel, destStartSample, source, samples_to_write);

      // Advance source to next sample after the last sample we've already copied.
      source = &source[samples_to_write];
      samples_to_write = numSamples - samples_to_write;
      destStartSample = 0;
    }

    delayBuffer_.copyFrom(destChannel, destStartSample, source, samples_to_write);

    // Return the next write index.
    int next_sample = destStartSample + samples_to_write;
    return next_sample == size_ ? 0 : next_sample;
  }

  void clear() {
    delayBuffer_.clear();
  }

  int size() {
    return size_;
  }

  private:
    int size_;
    juce::AudioBuffer<float> delayBuffer_;
};