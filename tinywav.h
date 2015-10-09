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

 #ifndef _TINY_WAV_
 #define _TINY_WAV_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// http://soundfile.sapp.org/doc/WaveFormat/

typedef enum TinyWavChannelFormat {
  TW_INTERLEAVED, // channel buffer is interleaved e.g. [LRLRLRLR]
  TW_INLINE,      // channel buffer is inlined e.g. [LLLLRRRR]
  TW_SPLIT        // channel buffer is split e.g. [[LLLL],[RRRR]]
} TinyWavChannelFormat;

typedef struct TinyWav {
  FILE *f;
  int16_t numChannels;
  uint32_t totalFramesWritten;
} TinyWav;

int tinywav_new(TinyWav *tw, int16_t numChannels, int32_t samplerate, const char *path);

size_t tinywav_write_f(TinyWav *tw, void *f, int len, TinyWavChannelFormat fmt);

void tinywav_read(TinyWav *tw, float *const f, size_t len);

void tinywav_close(TinyWav *tw);

#ifdef __cplusplus
}
#endif

#endif // _TINY_WAV_
