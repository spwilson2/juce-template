#include <gtest/gtest.h>

#include <source/CircularAudioBuffer.h>

namespace audio_plugin_test {
TEST(CircularAudioBuffer, InBoundCopy) {
  CircularAudioBuffer buf(/*channels=*/1,  /*samples=*/10);
  float srcBuffer[3] = {2,8,1};
  buf.copyFrom(/*destChannel=*/0, /*destStartSample=*/5, srcBuffer, /*numSamples=*/3);
  EXPECT_EQ(srcBuffer[0], buf.getSample(/*channel=*/0, /*sample=*/5));
  EXPECT_EQ(srcBuffer[1], buf.getSample(/*channel=*/0, /*sample=*/6));
  EXPECT_EQ(srcBuffer[2], buf.getSample(/*channel=*/0, /*sample=*/7));
}

TEST(CircularAudioBuffer, OverBoundCopy) {
  CircularAudioBuffer buf(/*channels=*/1,  /*samples=*/10);
  float srcBuffer[3] = {2,8,1};
  buf.copyFrom(/*destChannel=*/0, /*destStartSample=*/9, srcBuffer, /*numSamples=*/3);
  EXPECT_EQ(srcBuffer[0], buf.getSample(/*channel=*/0, /*sample=*/9));
  EXPECT_EQ(srcBuffer[1], buf.getSample(/*channel=*/0, /*sample=*/0));
  EXPECT_EQ(srcBuffer[2], buf.getSample(/*channel=*/0, /*sample=*/1));
}

TEST(CircularAudioBuffer, ReadPointersBounds) {
  CircularAudioBuffer buf(/*channels=*/1,  /*samples=*/10);
  CircularAudioBufferReadPointer pointers[2];
  buf.getSplitReadPointers(0, 2, pointers);
  EXPECT_EQ(pointers[0].size, 8);
  EXPECT_EQ(pointers[1].size, 2);
  EXPECT_EQ(pointers[0].buffer, &pointers[1].buffer[2]);
}
} // namespace audio_plugin_test
