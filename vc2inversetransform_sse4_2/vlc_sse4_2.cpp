/*****************************************************************************
 * vlc_sse4_2.cpp : Variable Length Decoding functions: SSE4.2 version
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

#include "logger.hpp"
#include "internal.h"
#include "vlc.hpp"
#include "../vc2inversetransform_c/vlc_c.hpp"
#include <cstdio>
#include <immintrin.h>

#ifdef DEBUG
#include "debug.hpp"
#endif

#ifdef DEBUG_P_BLOCK
extern int DEBUG_P_JOB;
extern int DEBUG_P_SLICE_Y;
extern int DEBUG_P_SLICE_X;
extern int DEBUG_P_SLICE_W;
extern int DEBUG_P_SLICE_H;
#endif


/* These are used for decoding actual coded coefficients */
inline int decode_sse4_2(uint8_t *idata, int ilength, int32_t *odata, int olength) {
  int icounter = 1;
  int ocounter = 0;
  int32_t V = 0;
  int state;

  _mm_prefetch((char *)idata, _MM_HINT_T0);
  _mm_prefetch((char *)odata, _MM_HINT_T0);

  const __m128i ZERO = _mm_set1_epi8(0);

  const LUTEntry *next;
  if (ilength > 0)
    next = &VLCLUT[idata[0]];
  else
    next = &VLCLUT[0xFF];

  while (icounter < ilength && ocounter < olength) {
    __m128i E = _mm_load_si128((__m128i *)next);
    next  = &VLCLUT[(((int)_mm_extract_epi8(E, 0)) << 8) + idata[icounter++]];

    V <<= _mm_extract_epi8(E, 1); // preshift
    V +=  _mm_extract_epi8(E, 8); // val0

    __m128i A = _mm_unpackhi_epi8(ZERO, E);
    __m128i B = _mm_srai_epi32(_mm_unpacklo_epi16(ZERO, A), 24);
    __m128i C = _mm_srai_epi32(_mm_unpackhi_epi16(ZERO, A), 24);

    B = _mm_insert_epi32(B, (V - 1)*((int8_t)_mm_extract_epi8(E, 2)), 0);

    _mm_storeu_si128((__m128i *)&odata[ocounter],     B);
    _mm_storeu_si128((__m128i *)&odata[ocounter + 4], C);

    if (_mm_extract_epi8(E, 3))         // term
      V = _mm_extract_epi8(E, 4);       // V
    ocounter += _mm_extract_epi8(E, 5); // N
  }

  if (icounter < ilength) {
    return ilength - icounter;
  }

  if (ocounter < olength) {
    __m128i E = _mm_load_si128((__m128i *)next);
    state = (int)_mm_extract_epi8(E, 0);

    V <<= _mm_extract_epi8(E, 1); // preshift
    V +=  _mm_extract_epi8(E, 8); // val0

    __m128i A = _mm_unpackhi_epi8(ZERO, E);
    __m128i B = _mm_srai_epi32(_mm_unpacklo_epi16(ZERO, A), 24);
    __m128i C = _mm_srai_epi32(_mm_unpackhi_epi16(ZERO, A), 24);

    B = _mm_insert_epi32(B, (V - 1)*((int8_t)_mm_extract_epi8(E, 2)), 0);

    _mm_storeu_si128((__m128i *)&odata[ocounter],     B);
    _mm_storeu_si128((__m128i *)&odata[ocounter + 4], C);

    if (_mm_extract_epi8(E, 3))         // term
      V = _mm_extract_epi8(E, 4);       // V
    ocounter += _mm_extract_epi8(E, 5); // N
  }

  if (ocounter < olength) {
    __m128i TMP = ZERO;
    switch (state) {
    case STATE_DATA:
      V <<= 1;
      V += 1;
    case STATE_FOLLOW:
    case STATE_SIGN:
      _mm_insert_epi32(TMP, -(V - 1), 0);
    }
    _mm_storeu_si128((__m128i *)&odata[ocounter], TMP);
    ocounter = (ocounter + 4)&0xFFFFFFFC;
  }

  while (ocounter < olength) {
    _mm_store_si128((__m128i *)&odata[ocounter], ZERO);
    ocounter+=4;
  }

  return 0;
}


template<class T> void decode_slices_sse4_2(QuantisationMatrix *matrices,
                                       CodedSlice * const input,
                                       DecodedSlice ** scratch,
                                       int n_slices_x,
                                       int n_slices_y,
                                       VideoPlane **video_data,
                                       int slice_width,
                                       int slice_height,
                                       int depth,
                                       DequantiseFunction *dequant) {
  for (int i = 0; i < 16384; i += 64)
    _mm_prefetch(((char *)VLCLUT) + i, _MM_HINT_T0);

  for (int Y = 0; Y < n_slices_y; Y++) {
    for (int X = 0; X < n_slices_x; X++) {
      const int n = Y*n_slices_x + X;
      int padding = 0;
      padding += decode_sse4_2((uint8_t *)input[n].data[0], input[n].length[0], scratch[0]->data, scratch[0]->size);
      _mm_prefetch((char *)&matrices[input[n].qindex], _MM_HINT_T0);
      _mm_prefetch((char *)&video_data[0]->as<T>()[Y*slice_height*video_data[0]->stride + X*slice_width], _MM_HINT_T0);
      dequant[0](&matrices[input[n].qindex], scratch[0]->data,
                 &video_data[0]->as<T>()[Y*slice_height*video_data[0]->stride + X*slice_width], video_data[0]->stride,
                 slice_width, slice_height, depth);

      padding += decode_sse4_2((uint8_t *)input[n].data[1], input[n].length[1], scratch[1]->data, scratch[1]->size);
      _mm_prefetch((char *)&video_data[1]->as<T>()[Y*slice_height*video_data[1]->stride + X*slice_width], _MM_HINT_T0);
      dequant[1](&matrices[input[n].qindex], scratch[1]->data,
                 &video_data[1]->as<T>()[Y*slice_height*video_data[1]->stride + X*slice_width/2], video_data[1]->stride,
                 slice_width/2, slice_height, depth);

      padding += decode_sse4_2((uint8_t *)input[n].data[2], input[n].length[2], scratch[2]->data, scratch[2]->size);
      _mm_prefetch((char *)&video_data[2]->as<T>()[Y*slice_height*video_data[2]->stride + X*slice_width], _MM_HINT_T0);
      dequant[2](&matrices[input[n].qindex], scratch[2]->data,
                 &video_data[2]->as<T>()[Y*slice_height*video_data[2]->stride + X*slice_width/2], video_data[2]->stride,
                 slice_width/2, slice_height, depth);

      input[n].padding = padding;

#ifdef DEBUG_P_BLOCK_DEC
      {
        if (Y == 0 && X == 0) {
          printf("-----------------------------------------------------------------\n");
          printf("  Decoded\n");
          printf("-----------------------------------------------------------------\n");
          T *D = scratch[0]->data;
          for (int y = 0; y < slice_height; y++) {
            for (int x = 0; x < slice_width; x++)
              printf("  %+6d", D[y*slice_width + x]);
            printf("\n");
          }
          printf("-----------------------------------------------------------------\n");
        }

        if (Y == 0 && X == 0) {
          uint32_t D[slice_height*slice_width];
          int n = 0;
          int skip = 1 << depth;
          for (int y = 0; y < slice_height; y += skip) {
            for (int x = 0; x < slice_width; x += skip) {
              D[y*slice_width + x] = scratch[0]->data[n++];
            }
          }

          for (int l = 0; l < depth; l++) {
            for (int y = 0; y < slice_height; y += skip) {
              for (int x = 0; x < slice_width; x += skip) {
                D[(y + 0*skip/2)*slice_width + x + 1*skip/2] = scratch[0]->data[n++];
              }
            }

            for (int y = 0; y < slice_height; y += skip) {
              for (int x = 0; x < slice_width; x += skip) {
                D[(y + 1*skip/2)*slice_width + x + 0*skip/2] = scratch[0]->data[n++];
              }
            }

            for (int y = 0; y < slice_height; y += skip) {
              for (int x = 0; x < slice_width; x += skip) {
                D[(y + 1*skip/2)*slice_width + x + 1*skip/2] = scratch[0]->data[n++];
              }
            }

            skip /= 2;
          }

          printf("-----------------------------------------------------------------\n");
          printf("  Reordered\n");
          printf("-----------------------------------------------------------------\n");
          for (int y = 0; y < slice_height; y++) {
            for (int x = 0; x < slice_width; x++)
              printf("  %+6d", D[y*slice_width + x]);
            printf("\n");
          }
          printf("-----------------------------------------------------------------\n");
        }
      }
 #endif
    }
  }
}

SliceDecoderFunc get_slice_decoder_sse4_2(int sample_size) {
  if (sample_size == 4) {
    return decode_slices_sse4_2<int32_t>;
  } else if (sample_size == 2) {
    return decode_slices_sse4_2<int16_t>;
  }

  return get_slice_decoder_c(sample_size);
}
