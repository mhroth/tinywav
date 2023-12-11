
#include <catch2/catch.hpp>
#include "tinywav.h"

#include <cstring> // for memset
#include "TestCommon.hpp"


static auto basedir = std::string(TOSTRING(SOURCE_DIR)) + "/test/test-data/";

TEST_CASE("Read 'standard' 16 bit int wave files")
{
  TinyWav tw;
  
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_16bitInt-mono.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_INT16);
  REQUIRE(tw.numChannels == 1);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 1);
  REQUIRE(tw.h.AudioFormat == 1);
  REQUIRE(tw.h.BitsPerSample == 16);
  REQUIRE(tw.h.BlockAlign == 2);
  REQUIRE(tw.h.ByteRate == 96000);
  REQUIRE(tw.h.Subchunk2Size == 9478);
  REQUIRE(tw.numFramesInHeader == 4739);
  
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_16bitInt-8ch.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_INT16);
  REQUIRE(tw.numChannels == 8);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 8);
  REQUIRE(tw.h.AudioFormat == 1);
  REQUIRE(tw.h.BlockAlign == 16);
  REQUIRE(tw.h.BitsPerSample == 16);
  REQUIRE(tw.h.ByteRate == 768000);
  REQUIRE(tw.h.Subchunk2Size == 76800);
  REQUIRE(tw.numFramesInHeader == 4800);
}

TEST_CASE("Read 'standard' 32 bit int wave files")
{
  TinyWav tw;
  // Supported, but data will be read as 32 bit float and needs to be converted
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitInt-mono.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_FLOAT32); // the hacky way it's currently supported
  REQUIRE(tw.numChannels == 1);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 1);
  REQUIRE(tw.h.AudioFormat == 1);
  REQUIRE(tw.h.BitsPerSample == 32);
  REQUIRE(tw.h.BlockAlign == 4);
  REQUIRE(tw.h.ByteRate == 192000);
  REQUIRE(tw.h.Subchunk2Size == 384004);
  REQUIRE(tw.numFramesInHeader == 96001);
}

TEST_CASE("Read 'standard' 32 bit float wave files")
{
  TinyWav tw;
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-stereo.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_FLOAT32); // the hacky way it's currently supported
  REQUIRE(tw.numChannels == 2);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 2);
  REQUIRE(tw.h.AudioFormat == 3);
  REQUIRE(tw.h.BitsPerSample == 32);
  REQUIRE(tw.h.BlockAlign == 8);
  REQUIRE(tw.h.ByteRate == 384000);
  REQUIRE(tw.h.Subchunk2Size == 768008);
  REQUIRE(tw.numFramesInHeader == 96001);
  
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-stereo-44k1.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_FLOAT32); // the hacky way it's currently supported
  REQUIRE(tw.numChannels == 2);
  REQUIRE(tw.h.SampleRate == 44100);
  REQUIRE(tw.h.NumChannels == 2);
  REQUIRE(tw.h.AudioFormat == 3);
  REQUIRE(tw.h.BitsPerSample == 32);
  REQUIRE(tw.h.BlockAlign == 8);
  REQUIRE(tw.h.ByteRate == 352800);
  REQUIRE(tw.h.Subchunk2Size == 705608);
  REQUIRE(tw.numFramesInHeader == 88201);
  
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "example_32bitFloat-8ch.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_FLOAT32); // the hacky way it's currently supported
  REQUIRE(tw.numChannels == 8);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 8);
  REQUIRE(tw.h.AudioFormat == 3);
  REQUIRE(tw.h.BitsPerSample == 32);
  REQUIRE(tw.h.BlockAlign == 32);
  REQUIRE(tw.h.ByteRate == 1536000);
  REQUIRE(tw.h.Subchunk2Size == 1012992);
  REQUIRE(tw.numFramesInHeader == 31656);
}

TEST_CASE("Read wave file with a lot of metadata afer fmt")
{
  TinyWav tw;
  REQUIRE(tinywav_open_read(&tw, std::string(basedir + "testfile-lot-of-metadata-after-fmt.wav").c_str(), TW_INTERLEAVED) == 0);
  tinywav_close_read(&tw);
  REQUIRE(tw.sampFmt == TW_FLOAT32);
  REQUIRE(tw.numChannels == 2);
  REQUIRE(tw.h.SampleRate == 48000);
  REQUIRE(tw.h.NumChannels == 2);
  REQUIRE(tw.h.AudioFormat == 3);
  REQUIRE(tw.h.BitsPerSample == 32);
  REQUIRE(tw.h.BlockAlign == 8);
  REQUIRE(tw.h.ByteRate == 384000);
  REQUIRE(tw.h.Subchunk2Size == 768000);
  REQUIRE(tw.numFramesInHeader == 96000);
}
  REQUIRE(tw.numFramesInHeader == 96000);
}
