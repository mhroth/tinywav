/**
 * Copyright (c) 2015, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "tinywav.h"

#include <assert.h>

#define NUM_CHANNELS 2
#define SAMPLE_RATE 48000
#define BLOCK_SIZE 512

int main(int argc, char *argv[]) {
  char* outputPath = "output.wav";
  int totalNumSamples;
  int framesProcessed = 0;
  
  if (argc < 2) return -1;
  const char *inputPath = argv[1];

  TinyWav twReader, twWriter;

  tinywav_open_read(&twReader, inputPath, TW_INLINE);
  if (twReader.numChannels != NUM_CHANNELS || twReader.h.SampleRate != SAMPLE_RATE) {
      printf("Supplied test wav has wrong format - should be [%d]channels, fs=[%d]\n", NUM_CHANNELS, SAMPLE_RATE);
      return -1;
  }
  totalNumSamples = twReader.numFramesInHeader;
  
  tinywav_open_write(&twWriter, NUM_CHANNELS, SAMPLE_RATE, TW_FLOAT32, TW_INLINE, outputPath);

  assert(totalNumSamples >= BLOCK_SIZE && "input file needs to be at least 1 block long!");
  while (framesProcessed < totalNumSamples) {
    int samplesRead, samplesWritten;
    int effectiveBlockSize = BLOCK_SIZE;
    float buffer[NUM_CHANNELS * BLOCK_SIZE];
    
    samplesRead = tinywav_read_f(&twReader, buffer, BLOCK_SIZE);
    assert(samplesRead > 0 && " Could not read from file!");
    
    if (samplesRead < BLOCK_SIZE) {
        effectiveBlockSize = samplesRead; // last block
    }
    
    samplesWritten = tinywav_write_f(&twWriter, buffer, effectiveBlockSize);
    assert(samplesWritten > 0 && "Could not write to file!");
    
    assert(samplesRead == samplesWritten && "mismatch in samples read/written to file!");
    framesProcessed += samplesRead * NUM_CHANNELS;
  }

  tinywav_close_read(&twReader);
  tinywav_close_write(&twWriter);

  return 0;
}
