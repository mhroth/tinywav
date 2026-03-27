
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
