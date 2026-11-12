# Third-party code in the Linux layer

## stb_truetype

`stb_truetype.h` is version 1.26 by Sean Barrett, copied from ee-macos `kit-src/third_party/stb/` (sha256
`ecd30b05e0dd4fea3a13c26810dd9e1992dc379049482c393d5a19e6b5090aab`). It is dual-licensed: public domain or the MIT
licence below.

```
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
```

## ee-macos

`GdiTrueType.h`, `GdiTrueType.cpp` and `GdiFontDB.cpp` are adapted from ee-macos
(https://github.com/maxgraf96/ee-macos) at commit 136d78fdf6608c869f0621942c7dfeba144a74e4. The first lines of each file
say what changed. `Miles.cpp` takes Miles 6.1's rules from ee-macos `kit-src/dx/mss32.cpp` at the same commit: the
preference defaults, the pan law, and the rules for loop counts, starts and positions. `DirectMusic.cpp` is adapted
from ee-macos `kit-src/dx/dmusic.cpp` and `kit-src/dx/dmusic_reverb.cpp` at the same commit: the COM surface over
dmusic, the output stage behind the port and the reverb. The `dmusic/` folder is ee-macos's copy of GothicKit/dmusic,
with ee-macos's patches.

The font metrics test data in `tests/port/linux/data/` are copied unchanged from ee-macos at the same commit: `gdi_metrics_windows11.jsonl`, a Windows 11 capture of 169 font requests, from
`kit-src/runtime/tests/data/`, and `cases_windows11.txt`, the requests, from `kit-src/tools/gdi_metrics/`.

```
MIT License

Copyright (c) 2026 Max Graf

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## GothicKit/dmusic

`dmusic/` is GothicKit/dmusic (https://github.com/GothicKit/dmusic) at commit
`b73b270b94714b8ec8d9d0b2a9f3a5c445da52d1`, with the patches that `dmusic/README.md` lists. `dmusic/vendor/TinySoundFont`
is GothicKit's modified copy of TinySoundFont by Bernhard Schelling (MIT) under the same terms.

```
The MIT License (Modern Variant)
Copyright © 2024. GothicKit Contributors

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in
all copies of this software.

IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
```

## GeneralUser GS

The repository does not hold a General MIDI bank. `tools/fetch_port_toolchain.py` downloads GeneralUser GS v2.0.3 by
S. Christian Collins (https://www.schristiancollins.com) to `toolchain/soundfonts/`, with its licence as
`GeneralUser-GS-LICENSE.txt`. The licence lets anyone use the bank, modify it and ship it in software. Its author asks
that a website does not link to his download files directly, but links to his website or holds its own copy.
