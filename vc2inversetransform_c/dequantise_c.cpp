/*****************************************************************************
 * dequantise_c.cpp : dequantiser functions: plain C++ version
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
#include "dequantise_c.hpp"

#define sgn(x) (((x)==0)?0:(((x)>0)?1:-1))

template <int slice_width,
          int slice_height,
          int depth,
          class T> void dequantise_c(QuantisationMatrix *qmatrix,
                                     int32_t *idata,
                                     void *_odata,
                                     int ostride,
                                     int, int, int) {
  T *odata = (T *)_odata;

  const int Y = 0;
  const int X = 0;
  const int N = 0;

  T * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  int skip = 1 << depth;
  int32_t qf = *(int32_t *)&qmatrix->qfactor[0][0];
  int32_t qo = *(int32_t *)&qmatrix->qoffset[0][0];
  for (int y = 0; y < slice_height; y += skip) {
    for (int x = 0; x < slice_width; x += skip) {
      int32_t D = *iptr++;
      optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
    }
  }

  for (int l = 1; l <= depth; l++) {
    qf = *(int32_t *)&qmatrix->qfactor[l][1];
    qo = *(int32_t *)&qmatrix->qoffset[l][1];
    for (int y = 0; y < slice_height; y += skip) {
      for (int x = skip/2; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    qf = *(int32_t *)&qmatrix->qfactor[l][2];
    qo = *(int32_t *)&qmatrix->qoffset[l][2];
    for (int y = skip/2; y < slice_height; y += skip) {
      for (int x = 0; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    qf = *(int32_t *)&qmatrix->qfactor[l][3];
    qo = *(int32_t *)&qmatrix->qoffset[l][3];
    for (int y = skip/2; y < slice_height; y += skip) {
      for (int x = skip/2; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    skip /= 2;
  }
}

template<class T> void dequantise_fallback_c(QuantisationMatrix *qmatrix,
                                             int32_t *idata,
                                             void *_odata,
                                             int ostride,
                                             int slice_width,
                                             int slice_height,
                                             int depth) {
  T *odata = (T *)_odata;

  const int Y = 0;
  const int X = 0;
  const int N = 0;

  T * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  int skip = 1 << depth;
  int32_t qf = *(int32_t *)&qmatrix->qfactor[0][0];
  int32_t qo = *(int32_t *)&qmatrix->qoffset[0][0];
  for (int y = 0; y < slice_height; y += skip) {
    for (int x = 0; x < slice_width; x += skip) {
      int32_t D = *iptr++;
      optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
    }
  }

  for (int l = 1; l <= depth; l++) {
    qf = *(int32_t *)&qmatrix->qfactor[l][1];
    qo = *(int32_t *)&qmatrix->qoffset[l][1];
    for (int y = 0; y < slice_height; y += skip) {
      for (int x = skip/2; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    qf = *(int32_t *)&qmatrix->qfactor[l][2];
    qo = *(int32_t *)&qmatrix->qoffset[l][2];
    for (int y = skip/2; y < slice_height; y += skip) {
      for (int x = 0; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    qf = *(int32_t *)&qmatrix->qfactor[l][3];
    qo = *(int32_t *)&qmatrix->qoffset[l][3];
    for (int y = skip/2; y < slice_height; y += skip) {
      for (int x = skip/2; x < slice_width; x += skip) {
        int32_t D = *iptr++;
        optr[y*ostride + x] = sgn(D)*(((abs(D)*qf) + qo) >> 2);
      }
    }

    skip /= 2;
  }
}

DequantiseFunction getDequantiseFunction_c(int slice_width,
                                           int slice_height,
                                           int depth,
                                           int sample_size) {
  if (sample_size == 4) {
    if (depth == 2 &&
        slice_width == 8 &&
        slice_height == 8)
      return dequantise_c<8,8,2, int32_t>;
    else if (depth == 2 &&
             slice_width == 4 &&
             slice_height == 8)
      return dequantise_c<4,8,2, int32_t>;
    else if (depth == 3 &&
             slice_width == 32 &&
             slice_height == 8)
      return dequantise_c<32,8,3, int32_t>;
    else if (depth == 3 &&
             slice_width == 16 &&
             slice_height == 8)
      return dequantise_c<16,8,3, int32_t>;

    return dequantise_fallback_c<int32_t>;
  } else if (sample_size == 2) {
    if (depth == 2 &&
        slice_width == 8 &&
        slice_height == 8)
      return dequantise_c<8,8,2, int16_t>;
    else if (depth == 2 &&
             slice_width == 4 &&
             slice_height == 8)
      return dequantise_c<4,8,2, int16_t>;
    else if (depth == 3 &&
             slice_width == 32 &&
             slice_height == 8)
      return dequantise_c<32,8,3, int16_t>;
    else if (depth == 3 &&
             slice_width == 16 &&
             slice_height == 8)
      return dequantise_c<16,8,3, int16_t>;

    return dequantise_fallback_c<int16_t>;
  }

  writelog(LOG_ERROR, "%s:%d:  Invalid sample size\n", __FILE__, __LINE__);
  throw VC2DECODER_NOTIMPLEMENTED;
}
