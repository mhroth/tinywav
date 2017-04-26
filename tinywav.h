/**
 * Copyright (c) 2015-2017, Martin Roth (mhroth@gmail.com)
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

 #ifndef _TINY_WAV_
 #define _TINY_WAV_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// http://soundfile.sapp.org/doc/WaveFormat/
// http://www-mmsp.ece.mcgill.ca/documents/AudioFormats/WAVE/WAVE.html

typedef enum TinyWavChannelFormat {
  TW_INTERLEAVED, // channel buffer is interleaved e.g. [LRLRLRLR]
  TW_INLINE,      // channel buffer is inlined e.g. [LLLLRRRR]
  TW_SPLIT        // channel buffer is split e.g. [[LLLL],[RRRR]]
} TinyWavChannelFormat;

typedef enum TinyWavSampleFormat {
  TW_INT16 = 2,  // two byte signed integer
  TW_FLOAT32 = 4 // four byte IEEE float
} TinyWavSampleFormat;

typedef struct TinyWav {
  FILE *f;
  int16_t numChannels;
  uint32_t totalFramesWritten;
  TinyWavChannelFormat chanFmt;
  TinyWavSampleFormat sampFmt;
} TinyWav;

int tinywav_new(TinyWav *tw,
    int16_t numChannels, int32_t samplerate,
    TinyWavSampleFormat sampFmt, TinyWavChannelFormat chanFmt,
    const char *path);

/**
 * Write sample data to file.
 *
 * @param tw  The TinyWav structure which has already been prepared.
 * @param f  A pointer to the sample data to write.
 * @param len  The number of frames to write.
 *
 * @return The total number of samples written to file.
 */
size_t tinywav_write_f(TinyWav *tw, void *f, int len);

/** Stop writing to the file. The Tinywav struct is now invalid. */
void tinywav_close(TinyWav *tw);

/** Returns true if the Tinywav struct is available to write. False otherwise. */
bool tinywav_isOpen(TinyWav *tw);

#ifdef __cplusplus
}
#endif

#endif // _TINY_WAV_
