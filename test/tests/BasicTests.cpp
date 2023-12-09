
#include <catch2/catch.hpp>

#include "tinywav.h"

#define NUM_CHANNELS 1
#define SAMPLE_RATE 48000

TEST_CASE("Tinywav - Basic Tests")
{
  TinyWav tw;
  tinywav_open_write(&tw,
      NUM_CHANNELS,
      SAMPLE_RATE,
      TW_FLOAT32, // the output samples will be 32-bit floats. TW_INT16 is also supported
      TW_INLINE,  // the samples to be written will be assumed to be inlined in a single buffer.
                  // Other options include TW_INTERLEAVED and TW_SPLIT
      "output.wav" // the output path
  );

  for (int i = 0; i < 100; i++) {
    // samples are always expected in float32 format,
    // regardless of file sample format
    float samples[480 * NUM_CHANNELS];
    tinywav_write_f(&tw, samples, sizeof(samples));
  }

  tinywav_close_write(&tw);
}
