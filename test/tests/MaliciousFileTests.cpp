
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
  SECTION("Reading") {
    TinyWav tw;
    REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-mono.wav").c_str(), TW_INTERLEAVED) == 0);
    int numFramesToRead = tw.numFramesInHeader;
    
    SECTION("try to read all samples declared in header at once") {
      float* buffer = (float*)malloc(numFramesToRead*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == numFramesToRead);
      free(buffer);
    }
    SECTION("try to read more samples than declared in header") {
      numFramesToRead += 4; // too much!
      float* buffer = nullptr; // should fail before trying to write to buffer
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == -1);
    }
#if TINYWAV_USE_ALLOCA
    SECTION("trigger alloca safeguard") {
      float* buffer = nullptr; // should fail before trying to write to buffer
      tw.numChannels = 32; // overwrite with another number of channels to trigger safeguard
      REQUIRE(tinywav_read_f(&tw, buffer, numFramesToRead) == -1);
    }
#endif

    tinywav_close_read(&tw);
  }


  SECTION("writing") {
    if (TestCommon::fileExists("bogus.wav")) {
      REQUIRE(std::remove("bogus.wav") == 0);
    }
    
    TinyWav tw;
    REQUIRE(tinywav_open_write(&tw, 16, 8000, TW_FLOAT32, TW_INLINE, "bogus.wav") == 0);
    int maxAllowedNumFrames16ch = 8*1024; // max 16ch, 8kSamples
   
    SECTION("try to write max samples") {
      float* buffer = (float*)malloc(maxAllowedNumFrames16ch*tw.numChannels*sizeof(float));
      REQUIRE(tinywav_write_f(&tw, buffer, maxAllowedNumFrames16ch) == maxAllowedNumFrames16ch);
      free(buffer);
    }
#if TINYWAV_USE_ALLOCA
    SECTION("trigger alloca safeguard") {
      maxAllowedNumFrames16ch += 4; // too much!
      float* buffer = nullptr; // should fail before trying to write to buffer
      REQUIRE(tinywav_write_f(&tw, buffer, maxAllowedNumFrames16ch) == -1);
    }
#endif

    tinywav_close_write(&tw);
  }
}
