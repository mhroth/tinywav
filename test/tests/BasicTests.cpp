
#include <catch2/catch.hpp>
#include "tinywav.h"

#include <cstring> // for memset
#include "TestCommon.hpp"

// for hton
#if _WIN32
  #include <winsock.h>
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <netinet/in.h>
#endif


TEST_CASE("Tinywav - Basic Reading/Writing Loop", "aka Eat Your Own Dog Food")
{
  const int numChannels = GENERATE(1, 2, 8);
  const int sampleRate = GENERATE(44100, 48000);
  constexpr int numSamples = 32; // 0.1 seconds @ 48k
  constexpr int blockSize = 16; // number of samples to read/write at a time

  const int frameSize = blockSize * numChannels;

  const char* testFile = "testFile.wav";

  TinyWavSampleFormat sampleFormat = GENERATE(TW_FLOAT32, TW_INT16);
  TinyWavChannelFormat channelFormatW = GENERATE(TW_INTERLEAVED, TW_INLINE, TW_SPLIT);
  TinyWavChannelFormat channelFormatR = GENERATE(TW_INTERLEAVED, TW_INLINE, TW_SPLIT);

  const int numBlocks = std::ceil(static_cast<float>(numSamples/blockSize));
  const auto bytesPerSample = static_cast<int>(sampleFormat);

  // Test data
  const std::vector<float> samples = TestCommon::createRandomVector(numSamples*numChannels);

  CAPTURE(numChannels, sampleRate, sampleFormat, channelFormatR, channelFormatW);
  
  TinyWav tw;

  REQUIRE(tinywav_open_write(&tw, numChannels, sampleRate, sampleFormat, channelFormatW, testFile) == 0);
  REQUIRE(tinywav_isOpen(&tw));
  REQUIRE(tw.numChannels == numChannels);
  REQUIRE(tw.sampFmt == sampleFormat);
  REQUIRE(tw.chanFmt == channelFormatW);
  REQUIRE(tw.f != nullptr);
  REQUIRE(tw.totalFramesReadWritten == 0);
  REQUIRE(tw.numFramesInHeader == -1); // not used when writing

  int frameCount = 0;
  for (int i = 0; i < numBlocks; i++) {
    std::vector<float> writeBuffer(samples.data() + i*frameSize, samples.data() + (i+1)*frameSize);
    if (channelFormatW == TW_SPLIT) {
      std::vector<float> deinterleaved = TestCommon::deinterleave(writeBuffer, numChannels);
      std::vector<float*> splitBuffer(numChannels);
      for (int c = 0; c < numChannels; ++c) {
        splitBuffer[c] = deinterleaved.data() + c * blockSize;
      }
      REQUIRE(tinywav_write_f(&tw, splitBuffer.data(), blockSize) == blockSize);
    } else {
      REQUIRE(tinywav_write_f(&tw, writeBuffer.data(), blockSize) == blockSize);
    }

    frameCount += blockSize;
    REQUIRE(tw.totalFramesReadWritten == frameCount);
    REQUIRE(tinywav_isOpen(&tw));
  }

  tinywav_close_write(&tw);
  REQUIRE(tw.f == nullptr);
  REQUIRE_FALSE(tinywav_isOpen(&tw));
  REQUIRE(tw.totalFramesReadWritten == frameCount);

  // Wipe struct in between (test with and without)
  if (GENERATE(true, false)) {
    memset(&tw, 0, sizeof(TinyWav));
  }

  // open same file and verify contents
  REQUIRE(tinywav_open_read(&tw, testFile, channelFormatR) == 0);
  REQUIRE(tw.numChannels == numChannels);
  REQUIRE(tw.sampFmt == sampleFormat);
  REQUIRE(tw.chanFmt == channelFormatR);
  REQUIRE(tw.f != nullptr);
  REQUIRE(tw.numFramesInHeader == numSamples);
  REQUIRE(tw.totalFramesReadWritten == 0);

  // verify header
  REQUIRE(tw.h.ChunkID == htonl(0x52494646)); // "RIFF"
  int s = sizeof(TinyWavHeader);
  REQUIRE(tw.h.ChunkSize == frameCount * numChannels * bytesPerSample + 36); // TODO: 36?
  REQUIRE(tw.h.Format == htonl(0x57415645)); // "WAVE"
  REQUIRE(tw.h.Subchunk1ID == htonl(0x666d7420)); // "fmt "
  REQUIRE(tw.h.Subchunk1Size == 16); // PCM
  REQUIRE(tw.h.AudioFormat == (int)sampleFormat-1); // 1 PCM, 3 IEEE float
  REQUIRE(tw.h.NumChannels == numChannels);
  REQUIRE(tw.h.SampleRate == sampleRate);
  REQUIRE(tw.h.ByteRate == sampleRate * numChannels * bytesPerSample);
  REQUIRE(tw.h.BlockAlign == numChannels * bytesPerSample);
  REQUIRE(tw.h.BitsPerSample == bytesPerSample * 8);
  REQUIRE(tw.h.Subchunk2ID == htonl(0x64617461)); // "data"
  REQUIRE(tw.h.Subchunk2Size == frameCount * numChannels * bytesPerSample);

  std::vector<float> readSamples;
  frameCount = 0;
  for (int i = 0; i < numBlocks; i++) {
    std::vector<float> readBuffer(frameSize);
    
    if (channelFormatR == TW_SPLIT) {
      std::vector<float*> splitBuffer(numChannels);
      for (int c = 0; c < numChannels; ++c) {
        splitBuffer[c] = readBuffer.data() + c * blockSize;
      }
      REQUIRE(tinywav_read_f(&tw, splitBuffer.data(), blockSize) == blockSize);
      readBuffer = TestCommon::interleave(readBuffer, numChannels);
    } else {

      REQUIRE(tinywav_read_f(&tw, readBuffer.data(), blockSize) == blockSize);
    }

    frameCount += blockSize;
    REQUIRE(tw.totalFramesReadWritten == frameCount);
    REQUIRE(tinywav_isOpen(&tw));

    readSamples.insert(readSamples.end(), readBuffer.begin(), readBuffer.end());
  }

  tinywav_close_read(&tw);
  REQUIRE(tw.f == nullptr);
  REQUIRE_FALSE(tinywav_isOpen(&tw));

  // Verify content
  REQUIRE(samples.size() == readSamples.size());
  if (sampleFormat == TW_FLOAT32 && channelFormatR == channelFormatW) { // only the simple case; for now
    REQUIRE(samples == readSamples);
  }
}


TEST_CASE("Tinywav - Test Error Behaviour")
{
  
  TinyWavChannelFormat channelFormat = GENERATE(TW_INTERLEAVED, TW_INLINE, TW_SPLIT);
  TinyWav tw;
  // TODO: need to switch from assertion to return values to test more
//  REQUIRE(tinywav_open_read(&tw, "bogus.wav", channelFormat) != 0);
  
}