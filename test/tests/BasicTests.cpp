
#include <catch2/catch.hpp>

#include "tinywav.h"

#include "TestCommon.hpp"

TEST_CASE("Tinywav - Test Basic Reading/Writing TW_INTERLEAVED")
{
  constexpr int numChannels = 2;
  constexpr int sampleRate = 48000;
  constexpr int numSamples = 4800; // 0.1 seconds
  constexpr int blockSize = 64; // number of samples to read/write at a time
  
  constexpr int frameSize = blockSize * numChannels;
  constexpr int numBlocks = static_cast<float>(numSamples/blockSize);
  
  const char* testFile = "testFile.wav";
  
  std::vector<float> samples = TestCommon::createRandomVector(numSamples*numChannels);
  TinyWav tw;

  SECTION("Interleaved") {
    tinywav_open_write(&tw, numChannels, sampleRate, TW_FLOAT32, TW_INTERLEAVED, testFile);
    
    for (int i = 0; i < numBlocks; i++) {
      std::vector<float> writeBuffer(samples.data() + i*frameSize, samples.data() + (i+1)*frameSize);
      
      REQUIRE(tinywav_write_f(&tw, writeBuffer.data(), blockSize) == blockSize);
    }
    tinywav_close_write(&tw);
  }
  
  SECTION("Inline") {
    tinywav_open_write(&tw, numChannels, sampleRate, TW_FLOAT32, TW_INLINE, testFile);
    
    for (int i = 0; i < numBlocks; i++) {
      std::vector<float> writeBuffer(samples.data() + i*frameSize, samples.data() + (i+1)*frameSize);
      
      REQUIRE(tinywav_write_f(&tw, writeBuffer.data(), blockSize) == blockSize);
    }
    
    tinywav_close_write(&tw);
  }
  
}
