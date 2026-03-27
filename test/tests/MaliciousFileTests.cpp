
#include <catch2/catch.hpp>
#include "tinywav.h"

#include <cstring> // for memset
#include "TestCommon.hpp"


static auto basedir = std::string(TOSTRING(SOURCE_DIR)) + "/test/test-data/";

TEST_CASE("Tinywav - Test behaviour with malicious input data")
{
  TinyWav tw;
  // without proper handling, this test could trigger a division by zero
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "malicious_numChannels0.wav").c_str(), TW_INTERLEAVED) == -1);
  tinywav_close_read(&tw);
}

TEST_CASE("Tinywav - Test Safeguards")
{
  // This test assumes TW_USE_ALLOCA is enabled
  SECTION("Reading") {
    TinyWav tw;
    REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-stereo.wav").c_str(), TW_INTERLEAVED) == 0);
    int numFramesToRead = tw.numFramesInHeader;
    
    SECTION("try to read all samples than declared in header at once") {
      float* buffer = (float*)malloc(numFramesToRead*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == numFramesToRead);
      free(buffer);
    }
    SECTION("try to read more samples than declared in header") {
      numFramesToRead += 4; // too much!
      float* buffer = (float*)malloc(numFramesToRead*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == -1);
      free(buffer);
    }
    SECTION("trigger alloca safeguard") {
      float* buffer = (float*)malloc(numFramesToRead*tw.numChannels*sizeof(float));
      tw.numChannels = 32; // overwrite with another number of channels to trigger safeguard
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == -1);
      free(buffer);
    }
  }
  
  SECTION("writing") {
    TinyWav tw;
    REQUIRE(tinywav_open_write(&tw, 16, 8000, TW_FLOAT32, TW_INLINE, "bogus.wav") == 0);
    int maxAllowedNumFrames16ch = 16*1024; // max 16ch, 16kSamples
   
    SECTION("try to write max samples") {
      float* buffer = (float*)malloc(maxAllowedNumFrames16ch*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_write_f(&tw, buffer, maxAllowedNumFrames16ch) == maxAllowedNumFrames16ch);
      free(buffer);
    }
    SECTION("trigger alloca safeguard") {
      maxAllowedNumFrames16ch += 4; // too much!
      float* buffer = (float*)malloc(maxAllowedNumFrames16ch*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_write_f(&tw, buffer, maxAllowedNumFrames16ch) == -1);
      free(buffer);
    }
  }
}
