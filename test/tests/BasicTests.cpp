
#include <catch2/catch.hpp>
#include "tinywav.h"

#include <cstring> // for memset
#include "TestCommon.hpp"

TEST_CASE("Tinywav - Basic Reading/Writing Loop", "aka Eat Your Own Dog Food")
{
  const int numChannels = GENERATE(1, 2, 8);
  const int sampleRate = GENERATE(44100, 48000);
  constexpr int numSamples = 4800; // 0.1 seconds @ 48k
  constexpr int blockSize = 64; // number of samples to read/write at a time

  const int frameSize = blockSize * numChannels;

  const char* testFile = "testFile.wav";

  if (TestCommon::fileExists(testFile)) {
      REQUIRE(remove(testFile) == 0);
  }

  TinyWavSampleFormat sampleFormat = GENERATE(TW_FLOAT32, TW_INT16);
  TinyWavChannelFormat channelFormatW = GENERATE(TW_INTERLEAVED, TW_INLINE, TW_SPLIT);
  TinyWavChannelFormat channelFormatR = GENERATE(TW_INTERLEAVED, TW_INLINE, TW_SPLIT);

  const int numBlocks = static_cast<int>(std::ceil(static_cast<float>(numSamples)/blockSize));
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
  REQUIRE(tw.h.ChunkID[0] == 'R');
  REQUIRE(tw.h.ChunkID[1] == 'I');
  REQUIRE(tw.h.ChunkID[2] == 'F');
  REQUIRE(tw.h.ChunkID[3] == 'F');

  int headerSize = 4 /*WAVE*/ + 8 /*Subchunk1ID(fmt ) + SubChunkIDSize*/ + 16 /*Subchunk1*/ + 8 /*Subchunk1ID(data) + SubChunkIDSize*/;
  REQUIRE(tw.h.ChunkSize == headerSize + frameCount * numChannels * bytesPerSample);
  REQUIRE(tw.h.Format[0] == 'W');
  REQUIRE(tw.h.Format[1] == 'A');
  REQUIRE(tw.h.Format[2] == 'V');
  REQUIRE(tw.h.Format[3] == 'E');
  REQUIRE(tw.h.Subchunk1ID[0] == 'f');
  REQUIRE(tw.h.Subchunk1ID[1] == 'm');
  REQUIRE(tw.h.Subchunk1ID[2] == 't');
  REQUIRE(tw.h.Subchunk1ID[3] == ' ');
  REQUIRE(tw.h.Subchunk1Size == 16); // PCM
  REQUIRE(tw.h.AudioFormat == (int)sampleFormat-1); // 1 PCM, 3 IEEE float
  REQUIRE(tw.h.NumChannels == numChannels);
  REQUIRE(tw.h.SampleRate == sampleRate);
  REQUIRE(tw.h.ByteRate == sampleRate * numChannels * bytesPerSample);
  REQUIRE(tw.h.BlockAlign == numChannels * bytesPerSample);
  REQUIRE(tw.h.BitsPerSample == bytesPerSample * 8);
  REQUIRE(tw.h.Subchunk2ID[0] == 'd');
  REQUIRE(tw.h.Subchunk2ID[1] == 'a');
  REQUIRE(tw.h.Subchunk2ID[2] == 't');
  REQUIRE(tw.h.Subchunk2ID[3] == 'a');
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
  
  if (TestCommon::fileExists("bogus.wav")) {
    REQUIRE(std::remove("bogus.wav") == 0);
  }
  
  SECTION("Test _open_ functions") {
    REQUIRE(tinywav_open_read(NULL, NULL, channelFormat) != 0);
    REQUIRE(tinywav_open_read(NULL, "bogus.wav", channelFormat) != 0);
    REQUIRE(tinywav_open_read(&tw, "bogus.wav", channelFormat) != 0);
    tinywav_close_read(&tw);
    
    REQUIRE(tinywav_open_write(NULL, -1, -1, TW_FLOAT32, TW_INLINE, NULL) != 0);
    REQUIRE(tinywav_open_write(&tw, -1, -1, TW_FLOAT32, TW_INLINE, NULL) != 0);
    REQUIRE(tinywav_open_write(&tw, 2, -1, TW_FLOAT32, TW_INLINE, NULL) != 0);
    REQUIRE(tinywav_open_write(&tw, 2, -1, TW_FLOAT32, TW_INLINE, "bogus.wav") != 0);
  }
  
  SECTION("Test _write_f") {
    REQUIRE(tinywav_open_write(&tw, 2, 8000, TW_FLOAT32, TW_INLINE, "bogus.wav") == 0);
    REQUIRE(tinywav_write_f(NULL, NULL, -1) != 0);
    REQUIRE(tinywav_write_f(&tw, NULL, -1) != 0);

    float buffer[128];
    REQUIRE(tinywav_write_f(NULL, buffer, -1) != 0);
    REQUIRE(tinywav_write_f(NULL, buffer, 16) != 0);
    REQUIRE(tinywav_write_f(&tw, buffer, 0) == 0);
    REQUIRE(tinywav_write_f(&tw, buffer, 16) == 16);
    REQUIRE(tinywav_write_f(&tw, buffer, 23) == 23);
    
    tinywav_close_write(&tw);
  }
  
  SECTION("Test _read_f") {
    constexpr int numChannels = 2;
    REQUIRE(tinywav_open_write(&tw, numChannels, 8000, TW_FLOAT32, TW_INTERLEAVED, "bogus.wav") == 0);
    tinywav_close_write(&tw);
    
    REQUIRE(tinywav_open_read(&tw, "bogus.wav", TW_INTERLEAVED) == 0);
    REQUIRE(tinywav_read_f(NULL, NULL, -1) != 0);
    REQUIRE(tinywav_read_f(&tw, NULL, -1) != 0);
    float buffer[128];
    REQUIRE(tinywav_read_f(&tw, buffer, -1) != 0);
    REQUIRE(tinywav_read_f(&tw, buffer, 0) == 0);
    REQUIRE(tinywav_read_f(&tw, buffer, 16) == 0); // no data in file yet!
    tinywav_close_read(&tw);

    // Test data
    constexpr int numSamples = 64;
    const std::vector<float> testData = TestCommon::createRandomVector(numChannels*numSamples);
    REQUIRE(tinywav_open_write(&tw, numChannels, 8000, TW_FLOAT32, TW_INTERLEAVED, "bogus.wav") == 0);
    REQUIRE(tinywav_write_f(&tw, (void*)testData.data(), numSamples) == numSamples);
    tinywav_close_write(&tw);
    
    REQUIRE(tinywav_open_read(&tw, "bogus.wav", TW_INTERLEAVED) == 0);
    REQUIRE(tinywav_read_f(&tw, buffer, 0) == 0); // reading 0 samples is a valid operation
    REQUIRE(tinywav_read_f(&tw, buffer, 16) == 16);
    REQUIRE(tinywav_read_f(&tw, buffer, numSamples-16-1) == numSamples-16-1); // leave one sample unread
    REQUIRE(tinywav_read_f(&tw, buffer, 1) == 1); // last sample
    REQUIRE(tinywav_read_f(&tw, buffer, 1) == 0); // no more data available
    
    tinywav_close_read(&tw);
  }
  
}
