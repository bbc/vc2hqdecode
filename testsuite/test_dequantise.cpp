/*****************************************************************************
 * test_dequantise.cpp : test dequantise functions
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
#include <stdlib.h>
#include <string.h>
#include "../vc2hqdecode/dequantise.hpp"
#include "../vc2inversetransform_c/dequantise_c.hpp"
#include "../vc2inversetransform_sse4_2/dequantise_sse4_2.hpp"
#include "randomiser.hpp"
#include "platform_variant.hpp"

struct dequantisetest_data {
  int qindex_min;
  int qindex_max;
  int slice_width;
  int slice_height;
  int depth;
  int sample_size;
};

dequantisetest_data DEQUANTISETEST_DATA[] = {
  { 0, 52,   8, 8, 2, 2 },
  { 0, 52,   4, 8, 2, 2 },
  { 0, 52,  32, 8, 3, 2 },
  { 0, 52,  16, 8, 3, 2 },
  { 0, 112,  8, 8, 2, 4 },
  { 0, 112,  4, 8, 2, 4 },
  { 0, 112, 32, 8, 3, 4 },
  { 0, 112, 16, 8, 3, 4 },
};
const int DEQUANTISETEST_DATA_NUM = sizeof(DEQUANTISETEST_DATA)/sizeof(dequantisetest_data);

int perform_dequantisetest(dequantisetest_data &data,
                           void *idata_pre,
                           bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  (void)idata_pre;
  (void)HAS_SSE4_2;(void)HAS_AVX;(void)HAS_AVX2;

  QuantisationMatrix *qmatrices = quantisation_matrices(VC2DECODER_WFT_LEGALL_5_3, data.depth, data.qindex_max);

  int r = 0;
  for (int qi = data.qindex_min; !r && qi <= data.qindex_max; qi++) {

    printf("%2dx%-2d (%1d): qindex=%3d ", data.slice_width, data.slice_height, data.depth, qi);
    if (data.sample_size == 2)
      printf("16-bit ");
    else
      printf("32-bit ");

    /* Get cfunc */

    printf(" C [ ");
    DequantiseFunction cfunc = NULL;
    try {
      cfunc = getDequantiseFunction_c(data.slice_width, data.slice_height, data.depth, data.sample_size);
    } catch(...) {
      printf(" NONE  ]\n");
      r = 1;
      break;
    }

    {
      /* Allocate input and output data buffers */
      int32_t *idata = (int32_t *)ALIGNED_ALLOC(32, data.slice_width*data.slice_height*sizeof(int32_t));
      void *cdata = ALIGNED_ALLOC(32, data.slice_width*data.slice_height*data.sample_size);
      void *tdata = ALIGNED_ALLOC(32, data.slice_width*data.slice_height*data.sample_size);

      for (int i = 0; i < data.slice_width*data.slice_height; i++) {
        idata[i] = ((int32_t *)idata_pre)[i] >> (((data.sample_size == 2)?17:1) + ((qi + 3)/4));
      }
      memset(cdata, 0, data.slice_width*data.slice_height*data.sample_size);
      memset(tdata, 0, data.slice_width*data.slice_height*data.sample_size);

      cfunc(&qmatrices[qi], idata, cdata, data.slice_width, data.slice_width, data.slice_height, data.depth);
      printf("  OK   ] ");



      if (HAS_SSE4_2) {
        printf(" SSE4.2 [ ");
        DequantiseFunction tfunc = NULL;
        try {
          tfunc = getDequantiseFunction_sse4_2(data.slice_width, data.slice_height, data.depth, data.sample_size);
        } catch(...) {
          printf(" ERROR ]\n");
          r = 1;
          goto out;
        }

        if (tfunc == cfunc) {
          printf(" NONE  ] ");
          goto out;
        }

        tfunc(&qmatrices[qi], idata, tdata, data.slice_width, data.slice_width, data.slice_height, data.depth);

        if (memcmp(cdata, tdata, data.slice_width*data.slice_height*data.sample_size)) {
          printf(" FAIL  ]\n");
          r = 1;
          goto out;
        } else {
          printf("  OK   ] ");
        }
      }

    out:

      ALIGNED_FREE(idata);
      ALIGNED_FREE(cdata);
      ALIGNED_FREE(tdata);
    }

    printf("\n");
  }

  return r;
}


int test_dequantise(bool HAS_SSE4_2, bool HAS_AVX, bool HAS_AVX2) {
  printf("--------------------------------------------------------------------------------\n");
  printf("  Inverse Quantise and Rearrange Tests\n");
  printf("\n");
  /* Load some input data for the tests */
  const int ilength = 4096;
  void *idata = ALIGNED_ALLOC(32, ilength);
  if (!randomiser((char *)idata, ilength)) {
    printf("Error Getting Random Data\n");
    return 1;
  }

  int r = 0;
  for (int i = 0; !r && i < DEQUANTISETEST_DATA_NUM; i++) {
    r = perform_dequantisetest(DEQUANTISETEST_DATA[i],
                               idata,
                               HAS_SSE4_2, HAS_AVX, HAS_AVX2);
  }

  printf("--------------------------------------------------------------------------------\n");

  ALIGNED_FREE(idata);
  return r;
}
