# TinyWav

TinyWav is a minimal WAV audio file library written in C.

## Code Example
### Writing
```C
#include "tinywav.h"

tinywav tw;
tinywav_new(&tw,
    1, 48000,
    TW_FLOAT32, TW_INLINE,
    "/Users/mhroth/Desktop/hello.wav");

for (int i = 0; i < 100; i++) {
  float samples[480];
  tinywav_write_f(&tw, samples, sizeof(samples));
}

tinywav_close(&tw);
```

## License
TinyWav is published under the [ISC license](http://opensource.org/licenses/ISC). Please see the `LICENSE` file included in this repository, also reproduced below. In short, you are welcome to use this code for any purpose, including commercial and closed-source use.

```
Copyright (c) 2015, Martin Roth <mhroth@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```
