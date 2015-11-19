/*****************************************************************************
 * vlc_c.cpp : Variable Length Decoding functions: plain C++ version
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
#include <cstdio>

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
void decode_simple(uint8_t *idata, int ilength, int32_t *odata, int olength) {
  int icounter = 0;
  int ocounter = 0;
  uint8_t state = STATE_START;
  int32_t V = 0;
  int bit_n = 7;
  uint8_t bit;

  while(ocounter < olength) {
    if (icounter < ilength) {
      bit = (idata[icounter] >> bit_n)&0x1;
      bit_n--;
      if (bit_n < 0) {
        icounter++;
        bit_n = 7;
      }
    } else {
      bit = 1;
    }

    switch(state) {
    case STATE_START:
      if (bit == 1) {
        odata[ocounter++] = (int32_t)0;
        state = STATE_START;
        V = 0;
        continue;
      } else {
        V = 1;
        state = STATE_DATA;
        continue;
      }
    case STATE_FOLLOW:
      if (bit == 1)
        state = STATE_SIGN;
      else
        state = STATE_DATA;
      continue;
    case STATE_DATA:
      V <<= 1;
      V += bit;
      state = STATE_FOLLOW;
      continue;
    case STATE_SIGN:
      odata[ocounter++] = ((bit==0)?1:-1)*((int32_t)(V - 1));
      V = 0;
      state = STATE_START;
      continue;
    }
  }
}

inline int decode_c(uint8_t *idata, int ilength, int32_t *odata, int olength) {
  int icounter = 1;
  int ocounter = 0;
  int32_t V = 0;
  int state;

  const LUTEntry *next;
  if (ilength > 0)
    next = &VLCLUT[idata[0]];
  else
    next = &VLCLUT[0xFF];

  while (icounter < ilength && ocounter < olength) {
    const LUTEntry &E = *next;
    next  = &VLCLUT[(((int)E.state) << 8) + idata[icounter++]];

    V <<= E.preshift;
    V |=  E.val0; // val0

    odata[ocounter + 0] = (V - 1)*E.sgn;
    odata[ocounter + 1] = E.val1;
    odata[ocounter + 2] = E.val2;
    odata[ocounter + 3] = E.val3;
    odata[ocounter + 4] = E.val4;
    odata[ocounter + 5] = E.val5;
    odata[ocounter + 6] = E.val6;
    odata[ocounter + 7] = E.val7;

    if (E.term)
      V = E.V;
    ocounter += E.N;
  }

  if (icounter < ilength) {
    return ilength - icounter;
  }

  if (ocounter < olength) {
    const LUTEntry &E = *next;
    state = E.state;

    V <<= E.preshift;
    V |=  E.val0;

    odata[ocounter + 0] = (V - 1)*E.sgn;
    odata[ocounter + 1] = E.val1;
    odata[ocounter + 2] = E.val2;
    odata[ocounter + 3] = E.val3;
    odata[ocounter + 4] = E.val4;
    odata[ocounter + 5] = E.val5;
    odata[ocounter + 6] = E.val6;
    odata[ocounter + 7] = E.val7;

    if (E.term)
      V = E.V;
    ocounter += E.N;
  }

  if (ocounter < olength) {
    switch (state) {
    case STATE_DATA:
      V <<= 1;
      V += 1;
    case STATE_FOLLOW:
    case STATE_SIGN:
      odata[ocounter + 0] = -(V - 1);
    }
    ocounter = (ocounter + 4)&0xFFFFFFFC;
  }

  while (ocounter < olength) {
    odata[ocounter + 0] = 0;
    odata[ocounter + 1] = 0;
    odata[ocounter + 2] = 0;
    odata[ocounter + 3] = 0;
    ocounter+=4;
  }

  return 0;
}


template<class T> void decode_slices_c(QuantisationMatrix *matrices,
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
      padding += decode_c((uint8_t *)input[n].data[0], input[n].length[0], scratch[0]->data, scratch[0]->size);
      _mm_prefetch((char *)&matrices[input[n].qindex], _MM_HINT_T0);
      _mm_prefetch((char *)&video_data[0]->as<T>()[Y*slice_height*video_data[0]->stride + X*slice_width], _MM_HINT_T0);
      dequant[0](&matrices[input[n].qindex], scratch[0]->data,
                 &video_data[0]->as<T>()[Y*slice_height*video_data[0]->stride + X*slice_width], video_data[0]->stride,
                 slice_width, slice_height, depth);

      padding += decode_c((uint8_t *)input[n].data[1], input[n].length[1], scratch[1]->data, scratch[1]->size);
      _mm_prefetch((char *)&video_data[1]->as<T>()[Y*slice_height*video_data[1]->stride + X*slice_width], _MM_HINT_T0);
      dequant[1](&matrices[input[n].qindex], scratch[1]->data,
                 &video_data[1]->as<T>()[Y*slice_height*video_data[1]->stride + X*slice_width/2], video_data[1]->stride,
                 slice_width/2, slice_height, depth);

      padding += decode_c((uint8_t *)input[n].data[2], input[n].length[2], scratch[2]->data, scratch[2]->size);
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

SliceDecoderFunc get_slice_decoder_c(int sample_size) {
  if (sample_size == 4) {
    return decode_slices_c<int32_t>;
  } else if (sample_size == 2) {
    return decode_slices_c<int16_t>;
  }

  writelog(LOG_ERROR, "%s:%d:  Invalid sample size\n", __FILE__, __LINE__);
  throw VC2DECODER_NOTIMPLEMENTED;
}
