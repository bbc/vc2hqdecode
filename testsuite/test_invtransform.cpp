/*****************************************************************************
 * test_invtransform.cpp : test the inverse transform functions
 *****************************************************************************
 * Copyright (C) 2014-2015 BBC
 *
 * Authors: James P. Weaver <james.barrett@bbc.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at ipstudio@bbc.co.uk.
 *****************************************************************************/

#include <stdint.h>
#include "tests.hpp"
#include <cstdio>
#include <vc2hqdecode/vc2hqdecode.h>
#include <vc2hqdecode/vc2hqdecodestrings.h>
#include <stdlib.h>
#include <string.h>
#include "../vc2inversetransform_c/invtransform_c.hpp"
#include "../vc2inversetransform_sse4_2/invtransform_sse4_2.hpp"
#include "randomiser.hpp"
#include "platform_variant.hpp"

struct invhtransformtest_data {
  int wavelet;
  int level;
  int depth;
  int sample_size;
  bool SSE4_2;
  bool AVX;
  bool AVX2;
};

struct invhtransformfinaltest_data {
  int wavelet;
  int active_bits;
  int sample_size;
  bool SSE4_2;
  bool AVX;
  bool AVX2;
};

struct invvtransformtest_data {
  int wavelet;
  int level;
  int depth;
  int sample_size;
  bool SSE4_2;
  bool AVX;
  bool AVX2;
};

invhtransformtest_data INVHTRANSFORMTEST_DATA[] = {
  /* Haar 0-shift */
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 3, 4, true, false, false },
  /* Haar 1-shift */
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 3, 4, true, false, false },
  /* LeGall 5,3 */
  { VC2DECODER_WFT_LEGALL_5_3, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 3, 4, true, false, false },
};
const int INVHTRANSFORMTEST_DATA_NUM = sizeof(INVHTRANSFORMTEST_DATA)/sizeof(invhtransformtest_data);

invhtransformfinaltest_data INVHTRANSFORMFINALTEST_DATA[] = {
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 10, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 10, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 10, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 10, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 10, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 10, 4, true, false, false },
};

const int INVHTRANSFORMFINALTEST_DATA_NUM = sizeof(INVHTRANSFORMFINALTEST_DATA)/sizeof(invhtransformfinaltest_data);

invvtransformtest_data INVVTRANSFORMTEST_DATA[] = {
  /* Haar 0-shift */
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 1, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 2, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 1, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 1, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_NO_SHIFT, 2, 3, 4, true, false, false },
  /* Haar 1-shift */
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 1, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 2, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 2, 3, 2, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 1, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 2, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 1, 3, 4, true, false, false },
  { VC2DECODER_WFT_HAAR_SINGLE_SHIFT, 2, 3, 4, true, false, false },
  /* LeGall 5,3 */
  { VC2DECODER_WFT_LEGALL_5_3, 0, 1, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 2, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 2, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 3, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 3, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 2, 3, 2, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 1, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 2, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 2, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 0, 3, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 1, 3, 4, true, false, false },
  { VC2DECODER_WFT_LEGALL_5_3, 2, 3, 4, true, false, false },
};
const int INVVTRANSFORMTEST_DATA_NUM = sizeof(INVVTRANSFORMTEST_DATA)/sizeof(invvtransformtest_data);

int perform_invhtransformtest(invhtransformtest_data &data,
                              void *idata_pre,
                              const int width,
                              const int height,
                              const int stride,
                              bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  int r = 0;
  (void)HAS_AVX;(void)HAS_AVX2;

  printf("%-20s: H %d/%d  ", VC2DecoderWaveletFilterTypeString[data.wavelet], data.level, data.depth);
  if (data.sample_size == 2)
    printf("16-bit ");
  else
    printf("32-bit ");

  /* Use C version to generate comparison value */
  printf("C [ ");
  void *cdata = ALIGNED_ALLOC(32, height*stride*data.sample_size);
  memcpy(cdata, idata_pre, height*stride*data.sample_size);
  InplaceTransform ctrans = NULL;
  try {
    ctrans = get_invhtransform_c(data.wavelet, data.level, data.depth, data.sample_size);
  } catch(...) {
    printf(" NONE ]");
    r = 1;
    goto out;
  }
  printf("EXISTS ] ");
  ctrans(cdata, stride, width, height);

  /* Test SSE4_2 version */
  if (HAS_SSE4_2 && data.SSE4_2) {
    printf("SSE4.2 [");
    InplaceTransform trans = get_invhtransform_sse4_2(data.wavelet, data.level, data.depth, data.sample_size);
    if (trans == ctrans) {
      printf("NONE]");
    } else {
      void *tdata = ALIGNED_ALLOC(32, height*stride*data.sample_size);
      memcpy(tdata, idata_pre, height*stride*data.sample_size);
      trans(tdata, stride, width, height);
      if (memcmp(cdata, tdata, height*stride*data.sample_size)) {
        printf("FAIL]\n");
        r = 1;
      } else {
        printf(" OK ] ");
      }
      ALIGNED_FREE(tdata);
    }
  }

 out:

  printf("\n");

  ALIGNED_FREE(cdata);

  return r;
}

struct offsets_t { int left; int right; int top; int bottom; };

int perform_invhtransformfinaltest(invhtransformfinaltest_data &data,
                                   void *idata_pre,
                                   const int width,
                                   const int height,
                                   const int stride,
                                   bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  int r = 0;
  (void)HAS_AVX;(void)HAS_AVX2;

  void *idata = ALIGNED_ALLOC(32, height*stride*data.sample_size);
  memcpy(idata, idata_pre, height*stride*data.sample_size);

  struct offsets_t offsets[] = { {  0,  0,  0,  0 },
                                 { 32,  0,  0,  0 },
                                 {  0, 32,  0,  0 },
                                 { 32, 32,  0,  0 },
                                 {  0,  0, 32,  0 },
                                 {  0,  0,  0, 32 },
                                 {  0,  0, 32, 32 },
                                 { 32,  0, 32,  0 },
                                 { 32,  0,  0, 32 },
                                 { 32,  0, 32, 32 },
                                 {  0, 32, 32,  0 },
                                 {  0, 32,  0, 32 },
                                 {  0,  0, 32, 32 },
                                 { 32, 32, 32,  0 },
                                 { 32, 32,  0, 32 },
                                 { 32, 32, 32, 32 },
  };
  for (int i = 0; r==0 && i < (int)(sizeof(offsets)/sizeof(struct offsets_t)); i++) {
    char *cdata = (char *)malloc(height*stride*sizeof(uint16_t));
    memset(cdata, 0, height*stride*sizeof(uint16_t));

    printf("%-20s: H 0/* (%2d,%2d,%2d,%2d) (active %d-bit)  ", VC2DecoderWaveletFilterTypeString[data.wavelet], offsets[i].left, offsets[i].right, offsets[i].top, offsets[i].bottom, data.active_bits);
    if (data.sample_size == 2)
      printf("16-bit ");
    else
      printf("32-bit ");

    /* Use C version to generate comparison value */
    printf("C [ ");
    InplaceTransformFinal ctrans = NULL;
    try {
      ctrans = get_invhtransformfinal_c(data.wavelet, data.active_bits, data.sample_size);
    } catch(...) {
      printf(" NONE ]");
      r = 1;
      free(cdata);
      break;
    }

    ctrans(idata, stride, cdata + (offsets[i].top*stride + offsets[i].left)*2, stride, width, height, offsets[i].left, offsets[i].top, width - offsets[i].left - offsets[i].right, height - offsets[i].top - offsets[i].bottom);
    if (memcmp(idata, idata_pre, height*stride*data.sample_size) != 0) {
      printf(" BAD  ]\n");
      printf("   c function overwrites input data!\n");
      r = 1;
      free(cdata);
      printf("\n");
      break;
    }
    for (int y = 0; y < offsets[i].top; y++) {
      for (int x = 0; x < stride; x++) {
        if (((uint16_t *)cdata)[y*stride + x] != 0) {
          printf(" BAD  ]\n");
          printf("   c function writes outside of specified memory area!\n");
          r = 1;
          free(cdata);
          printf("\n");
          break;
        }
      }
    }
    for (int y = offsets[i].top; y < height - offsets[i].bottom; y++) {
      for (int x = 0; x < offsets[i].left; x++) {
        if (((uint16_t *)cdata)[y*stride + x] != 0) {
          printf(" BAD  ]\n");
          printf("   c function writes outside of specified memory area!\n");
          r = 1;
          free(cdata);
          printf("\n");
          break;
        }
      }
      for (int x = offsets[i].left; x < width - offsets[i].right; x++) {
        if (((uint16_t *)cdata)[y*stride + x] >= (1 << data.active_bits)) {
          printf(" BAD  ]\n");
          printf("   c function does not clip values to correct number of bits!\n");
          r = 1;
          free(cdata);
          printf("\n");
          break;
        }
      }
      for (int x = width - offsets[i].right; x < stride; x++) {
        if (((uint16_t *)cdata)[y*stride + x] != 0) {
          printf(" BAD  ]\n");
          printf("   c function writes outside of specified memory area!\n");
          r = 1;
          free(cdata);
          printf("\n");
          break;
        }
      }
    }
    for (int y = height - offsets[i].bottom; y < height; y++) {
      for (int x = 0; x < stride; x++) {
        if (((uint16_t *)cdata)[y*stride + x] != 0) {
          printf(" BAD  ]\n");
          printf("   c function writes outside of specified memory area!\n");
          r = 1;
          free(cdata);
          printf("\n");
          break;
        }
      }
    }
    printf(" GOOD ] ");

    /* Test SSE4_2 version */
    if (HAS_SSE4_2 && data.SSE4_2) {
      printf("SSE4.2 [");
      InplaceTransformFinal trans = get_invhtransformfinal_sse4_2(data.wavelet, data.active_bits, data.sample_size);
      if (trans == ctrans) {
        printf("NONE]");
      } else {
        char *tdata = (char *)malloc(height*stride*sizeof(uint16_t));
        memset(tdata, 0, height*stride*sizeof(uint16_t));
        trans(idata, stride, tdata + (offsets[i].top*stride + offsets[i].left)*2, stride, width, height, offsets[i].left, offsets[i].top, width - offsets[i].left - offsets[i].right, height - offsets[i].top - offsets[i].bottom);
        if (memcmp(cdata, tdata, height*stride*sizeof(uint16_t))) {
          printf("FAIL]\n");
          r = 1;
        } else {
          printf(" OK ] ");
        }
        free(tdata);
      }
    }
    free(cdata);
    printf("\n");
  }

  ALIGNED_FREE(idata);

  return r;
}

int perform_invvtransformtest(invvtransformtest_data &data,
                              void *idata_pre,
                              const int width,
                              const int height,
                              const int stride,
                              bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  int r = 0;
  (void)HAS_AVX;(void)HAS_AVX2;

  printf("%-20s: V %d/%d  ", VC2DecoderWaveletFilterTypeString[data.wavelet], data.level, data.depth);
  if (data.sample_size == 2)
    printf("16-bit ");
  else
    printf("32-bit ");

  /* Use C version to generate comparison value */
  printf("C [ ");
  void *cdata = ALIGNED_ALLOC(32, height*stride*data.sample_size);
  memcpy(cdata, idata_pre, height*stride*data.sample_size);
  InplaceTransform ctrans = NULL;
  try {
    ctrans = get_invvtransform_c(data.wavelet, data.level, data.depth, data.sample_size);
  } catch(...) {
    printf(" NONE ]");
    r = 1;
    goto out;
  }
  printf("EXISTS ] ");
  ctrans(cdata, stride, width, height);

  /* Test SSE4_2 version */
  if (HAS_SSE4_2 && data.SSE4_2) {
    printf("SSE4.2 [");
    InplaceTransform trans = get_invvtransform_sse4_2(data.wavelet, data.level, data.depth, data.sample_size);
    if (trans == ctrans) {
      printf("NONE]");
    } else {
      void *tdata = ALIGNED_ALLOC(32, height*stride*data.sample_size);
      memcpy(tdata, idata_pre, height*stride*data.sample_size);
      trans(tdata, stride, width, height);
      if (memcmp(cdata, tdata, height*stride*data.sample_size)) {
        printf("FAIL]\n");
        r = 1;
      } else {
        printf(" OK ] ");
      }
      ALIGNED_FREE(tdata);
    }
  }
 out:

  printf("\n");

  ALIGNED_FREE(cdata);

  return r;
}


int test_invtransform(bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  printf("--------------------------------------------------------------------------------\n");
  printf("  Inverse Transform Tests\n");
  printf("\n");
  /* Load some input data for the tests */
  const int height = 1080;
  const int width  = 1920;
  const int stride = ((1920 + 1023)/1024)*1024;
  void *idata16 = ALIGNED_ALLOC(32, height*stride*sizeof(int16_t));
  void *idata32 = ALIGNED_ALLOC(32, height*stride*sizeof(int32_t));

  if (!randomiser((char *)idata16, height*stride*sizeof(int16_t))) {
    printf("Error Getting Random Data\n");
    return 1;
  }

  /* Need to make sure the samples aren't so large they overflow during the calculations */
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < stride; x++) {
      ((int16_t *)idata16)[y*stride + x] >>= 2;
      ((int32_t *)idata32)[y*stride + x] = ((int16_t *)idata16)[y*stride + x];
    }
  }
  /*for (int y = 0; y < height; y++) {
    for (int x = 0; x < stride; x++) {
      ((int32_t *)idata)[y*stride + x] = 1;
    }
    }*/

  int r = 0;
  for (int i = 0; !r && i < INVHTRANSFORMTEST_DATA_NUM; i++) {
    void * idata = (INVHTRANSFORMTEST_DATA[i].sample_size == 2)?idata16:idata32;
    r = perform_invhtransformtest(INVHTRANSFORMTEST_DATA[i],
                                  idata,
                                  width,
                                  height,
                                  stride,
                                  HAS_SSE4_2, HAS_AVX, HAS_AVX2);
  }

  for (int i = 0; !r && i < INVHTRANSFORMFINALTEST_DATA_NUM; i++) {
    void * idata = (INVHTRANSFORMFINALTEST_DATA[i].sample_size == 2)?idata16:idata32;
    r = perform_invhtransformfinaltest(INVHTRANSFORMFINALTEST_DATA[i],
                                       idata,
                                       width,
                                       height,
                                       stride,
                                       HAS_SSE4_2, HAS_AVX, HAS_AVX2);
  }

  for (int i = 0; !r && i < INVVTRANSFORMTEST_DATA_NUM; i++) {
    void * idata = (INVVTRANSFORMTEST_DATA[i].sample_size == 2)?idata16:idata32;
    r = perform_invvtransformtest(INVVTRANSFORMTEST_DATA[i],
                                  idata,
                                  width,
                                  height,
                                  stride,
                                  HAS_SSE4_2, HAS_AVX, HAS_AVX2);
  }

  ALIGNED_FREE(idata16);
  ALIGNED_FREE(idata32);

  printf("--------------------------------------------------------------------------------\n");
  return r;
}
