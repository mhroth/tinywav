
#include <catch2/catch.hpp>
#include "tinywav.h"

#include <cstring> // for memset
#include "TestCommon.hpp"


static auto basedir = std::string(TOSTRING(SOURCE_DIR)) + "/test/test-data/";

TEST_CASE("Read 'standard' 16 bit int wave files")
{
  TinyWav tw;
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_16bitInt-mono.wav").c_str(), TW_INTERLEAVED) == 0);
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_16bitInt-8ch.wav").c_str(), TW_INTERLEAVED) == 0);
}

TEST_CASE("Read 'standard' 32 bit int wave files")
{
  TinyWav tw;
  // Supported, but data will be read as 32 bit float and needs to be converted
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitInt-mono.wav").c_str(), TW_INTERLEAVED) == 0);
}

TEST_CASE("Read 'standard' 32 bit float wave files")
{
  TinyWav tw;
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-stereo.wav").c_str(), TW_INTERLEAVED) == 0);
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-stereo-44k1.wav").c_str(), TW_INTERLEAVED) == 0);
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-8ch.wav").c_str(), TW_INTERLEAVED) == 0);
}
