/* Copyright (c) 2011 The WebM project authors. All Rights Reserved. */
/*  */
/* Use of this source code is governed by a BSD-style license */
/* that can be found in the LICENSE file in the root of the source */
/* tree. An additional intellectual property rights grant can be found */
/* in the file PATENTS.  All contributing project authors may */
/* be found in the AUTHORS file in the root of the source tree. */
#include "vpx/vpx_codec.h"
static const char* const cfg = "--disable-vp8 --enable-vp9 --disable-shared --enable-static --enable-small --disable-webm-io --disable-internal-stats --disable-vp9-highbitdepth --disable-docs --prefix=/e/GIT/RSDKv5-Decompilation/dependencies/windows/libs_x86 --disable-vp9-encoder";
const char *vpx_codec_build_config(void) {return cfg;}
