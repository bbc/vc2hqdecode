/*****************************************************************************
 * legall_invtransform.hpp : LeGall filter inverse transform functions: 
 *                           SSE4.2 version
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

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif // _WIN32

template<int skip> void LeGall_5_3_invtransform_V_inplace_sse4_2_int32_t(void *_idata,
                                                                         const int istride,
                                                                         const int width,
                                                                         const int height) {
  int32_t *idata = (int32_t *)_idata;
  const __m128i ONE = _mm_set1_epi32(1);
  const __m128i TWO = _mm_set1_epi32(2);
  const int BLENDMASK = (skip == 1)?0x00:((skip == 2)?0xCC:0xFC);
  const int xskip = (skip > 4)?skip:4;

#define BLEND_FOR_WRITE(A,B) ((skip == 1)?(A):_mm_blend_epi16(A,B,BLENDMASK))

  __m128i Dm1, D, Dp1;
  int y = 0;
  int x = 0;
  __m128i Xm2, Xm1, X;

  for (int XX = 0; XX < width; XX += 1024) {
    y = 0;
    for (x = XX; x < width && x < XX + 1024; x+=xskip) {
      D   = _mm_load_si128((__m128i *)&idata[(y + 0*skip)*istride + x]);
      Dp1 = _mm_load_si128((__m128i *)&idata[(y + 1*skip)*istride + x]);
      Dm1 = Dp1;

      X   = _mm_sub_epi32(D, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(Dm1, Dp1) ,TWO), 2));
      _mm_store_si128((__m128i*)&idata[y*istride + x], BLEND_FOR_WRITE(X, D));
    }
    y += 2*skip;

    for (; y < height; y += 2*skip) {
      for (x = XX; x < width && x < XX + 1024; x+=xskip) {
        Xm2 = _mm_load_si128((__m128i *)&idata[(y - 2*skip)*istride + x]);
        Dm1 = _mm_load_si128((__m128i *)&idata[(y - 1*skip)*istride + x]);
        D   = _mm_load_si128((__m128i *)&idata[(y + 0*skip)*istride + x]);
        Dp1 = _mm_load_si128((__m128i *)&idata[(y + 1*skip)*istride + x]);

        X   = _mm_sub_epi32(D,   _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(Dm1, Dp1), TWO), 2));
        Xm1 = _mm_add_epi32(Dm1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(Xm2, X),   ONE), 1));

        _mm_store_si128((__m128i*)&idata[(y - 1*skip)*istride + x], BLEND_FOR_WRITE(Xm1, Dm1));
        _mm_store_si128((__m128i*)&idata[y*istride + x], BLEND_FOR_WRITE(X, D));
      }
    }

    for (x = XX; x < width && x < XX + 1024; x+=xskip) {
      Xm2 = _mm_load_si128((__m128i *)&idata[(y - 2*skip)*istride + x]);
      Dm1 = _mm_load_si128((__m128i *)&idata[(y - 1*skip)*istride + x]);
      X = Xm2;

      Xm1 = _mm_add_epi32(Dm1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(Xm2, X),   ONE), 1));

      _mm_store_si128((__m128i*)&idata[(y - 1*skip)*istride + x], BLEND_FOR_WRITE(Xm1, Dm1));
    }
  }

#undef BLEND_FOR_WRITE
}

template<int skip> void LeGall_5_3_invtransform_V_inplace_sse4_2_int16_t(void *_idata,
                                                                         const int istride,
                                                                         const int width,
                                                                         const int height) {
  int16_t *idata = (int16_t *)_idata;
  const __m128i ONE = _mm_set1_epi16(1);
  const __m128i TWO = _mm_set1_epi16(2);
  const int BLENDMASK = (skip == 1)?0x00:((skip == 2)?0xAA:((skip == 4)?0xEE:0xFE));
  const int xskip = (skip > 8)?skip:8;

#define BLEND_FOR_WRITE(A,B) ((skip == 1)?(A):_mm_blend_epi16(A,B,BLENDMASK))

  __m128i Dm1, D, Dp1;
  int y = 0;
  int x = 0;
  __m128i Xm2, Xm1, X;

  for (int XX = 0; XX < width; XX += 2048) {
    y = 0;
    for (x = XX; x < width && x < XX + 2048; x+=xskip) {
      D   = _mm_load_si128((__m128i *)&idata[(y + 0*skip)*istride + x]);
      Dp1 = _mm_load_si128((__m128i *)&idata[(y + 1*skip)*istride + x]);
      Dm1 = Dp1;

      X   = _mm_sub_epi16(D, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(Dm1, Dp1) ,TWO), 2));
      _mm_store_si128((__m128i*)&idata[y*istride + x], BLEND_FOR_WRITE(X, D));
    }
    y += 2*skip;

    for (; y < height; y += 2*skip) {
      for (x = XX; x < width && x < XX + 2048; x+=xskip) {
        Xm2 = _mm_load_si128((__m128i *)&idata[(y - 2*skip)*istride + x]);
        Dm1 = _mm_load_si128((__m128i *)&idata[(y - 1*skip)*istride + x]);
        D   = _mm_load_si128((__m128i *)&idata[(y + 0*skip)*istride + x]);
        Dp1 = _mm_load_si128((__m128i *)&idata[(y + 1*skip)*istride + x]);

        X   = _mm_sub_epi16(D,   _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(Dm1, Dp1), TWO), 2));
        Xm1 = _mm_add_epi16(Dm1, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(Xm2, X),   ONE), 1));

        _mm_store_si128((__m128i*)&idata[(y - 1*skip)*istride + x], BLEND_FOR_WRITE(Xm1, Dm1));
        _mm_store_si128((__m128i*)&idata[y*istride + x], BLEND_FOR_WRITE(X, D));
      }
    }

    for (x = XX; x < width && x < XX + 2048; x+=xskip) {
      Xm2 = _mm_load_si128((__m128i *)&idata[(y - 2*skip)*istride + x]);
      Dm1 = _mm_load_si128((__m128i *)&idata[(y - 1*skip)*istride + x]);
      X = Xm2;

      Xm1 = _mm_add_epi16(Dm1, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(Xm2, X),   ONE), 1));

      _mm_store_si128((__m128i*)&idata[(y - 1*skip)*istride + x], BLEND_FOR_WRITE(Xm1, Dm1));
    }
  }

#undef BLEND_FOR_WRITE
}


void LeGall_5_3_invtransform_H_inplace_1_sse4_2(void *_idata,
                                              const int istride,
                                              const int width,
                                              const int height) {
  int32_t *idata = (int32_t *)_idata;
  const __m128i ONE = _mm_set1_epi32(1);
  const __m128i TWO = _mm_set1_epi32(2);

  const int skip = 1;
  for (int y = 0; y < height; y+=skip) {
    int x = 0;

    __m128i D0, D4, D8, D12, E0, E8, Om1, O1, O9, O7, X, Y, ZE0, ZE2, ZE8, ZO1, Z0, Z4;
    D0 = _mm_load_si128((__m128i *)&idata[y*istride + x + 0]);
    D4 = _mm_load_si128((__m128i *)&idata[y*istride + x + 4]);
    _mm_prefetch((char *)&idata[y*istride + x + 8], _MM_HINT_T0);
    X  = _mm_unpacklo_epi32(D0, D4);
    Y  = _mm_unpackhi_epi32(D0, D4);
    E0 = _mm_unpacklo_epi32(X, Y);     // [  0  2  4  6 ]
    O1 = _mm_unpackhi_epi32(X, Y);     // [  1  3  5  7 ]
    Om1 = _mm_shuffle_epi32(O1, 0x90); // [  1  1  3  5 ]

    ZE0 = _mm_sub_epi32(E0, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O1, Om1), TWO), 2)); // {  0  2  4  6 }

    for (; x < width - 8; x += 8) {
      D8  = _mm_load_si128((__m128i *)&idata[y*istride + x +  8]);
      D12 = _mm_load_si128((__m128i *)&idata[y*istride + x + 12]);
      X  = _mm_unpacklo_epi32(D8, D12);
      Y  = _mm_unpackhi_epi32(D8, D12);
      E8 = _mm_unpacklo_epi32(X, Y);    // [  8 10 12 14 ]
      O9 = _mm_unpackhi_epi32(X, Y);    // [  9 11 13 15 ]
      O7 = _mm_alignr_epi8(O9, O1, 12); // [  7  9 11 13 ]

      ZE8 = _mm_sub_epi32(E8, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O9, O7), TWO), 2));   // {  8 10 12 14 }
      ZE2 = _mm_alignr_epi8(ZE8, ZE0, 4); // {  2  4  6  8 }
      ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }

      Z0 = _mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1); // {  0  1  2  3 }
      _mm_store_si128((__m128i *)&idata[y*istride + x + 0], Z0);
      Z4 = _mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1); // {  4  5  6  7 }
      _mm_store_si128((__m128i *)&idata[y*istride + x + 4], Z4);
      _mm_prefetch((char *)&idata[y*istride + x + 16], _MM_HINT_T0);

      O1  = O9;
      ZE0 = ZE8;
    }

    ZE2 = _mm_shuffle_epi32(ZE0, 0xF9); // {  2  4  6  6 }
    ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }

    Z0 = _mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1); // {  0  1  2  3 }
    _mm_store_si128((__m128i *)&idata[y*istride + x + 0], Z0);
    Z4 = _mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1); // {  4  5  6  7 }
    _mm_store_si128((__m128i *)&idata[y*istride + x + 4], Z4);

  }
}

template<int active_bits> void LeGall_5_3_invtransform_H_final_1_10_sse4_2_int32_t(void *_idata,
                                                                     const int istride,
                                                                     const char *odata,
                                                                     const int ostride,
                                                                     const int iwidth,
                                                                     const int iheight,
                                                                     const int ooffset_x,
                                                                     const int ooffset_y,
                                                                     const int owidth,
                                                                     const int oheight){
  int32_t *idata = (int32_t *)_idata;
  const __m128i OFFSET = _mm_set1_epi32((1 << (active_bits - 1)));
  const __m128i ONE = _mm_set1_epi32(1);
  const __m128i TWO = _mm_set1_epi32(2);

  const int skip = 1;
  for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y+=skip) {
    int x = 0;

    __m128i D0, D4, D8, D12, E0, E8, Om1, O1, O9, O7, X, Y, ZE0, ZE2, ZE8, ZO1, Z0, Z4, ZZ0;
    D0 = _mm_load_si128((__m128i *)&idata[y*istride + x + 0]);
    D4 = _mm_load_si128((__m128i *)&idata[y*istride + x + 4]);
    _mm_prefetch((char *)&idata[y*istride + x + 8], _MM_HINT_T0);
    X  = _mm_unpacklo_epi32(D0, D4);
    Y  = _mm_unpackhi_epi32(D0, D4);
    E0 = _mm_unpacklo_epi32(X, Y);     // [  0  2  4  6 ]
    O1 = _mm_unpackhi_epi32(X, Y);     // [  1  3  5  7 ]
    Om1 = _mm_shuffle_epi32(O1, 0x90); // [  1  1  3  5 ]

    ZE0 = _mm_sub_epi32(E0, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O1, Om1), TWO), 2)); // {  0  2  4  6 }

    for (; x < ooffset_x; x += 8) {
      D8  = _mm_load_si128((__m128i *)&idata[y*istride + x +  8]);
      D12 = _mm_load_si128((__m128i *)&idata[y*istride + x + 12]);
      X  = _mm_unpacklo_epi32(D8, D12);
      Y  = _mm_unpackhi_epi32(D8, D12);
      E8 = _mm_unpacklo_epi32(X, Y);    // [  8 10 12 14 ]
      O9 = _mm_unpackhi_epi32(X, Y);    // [  9 11 13 15 ]
      O7 = _mm_alignr_epi8(O9, O1, 12); // [  7  9 11 13 ]

      ZE8 = _mm_sub_epi32(E8, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O9, O7), TWO), 2));   // {  8 10 12 14 }
      _mm_prefetch((char *)&idata[y*istride + x + 16], _MM_HINT_T0);

      O1  = O9;
      ZE0 = ZE8;
    }

    for (; x < iwidth - 8 && x < ooffset_x + owidth; x += 8) {
      D8  = _mm_load_si128((__m128i *)&idata[y*istride + x +  8]);
      D12 = _mm_load_si128((__m128i *)&idata[y*istride + x + 12]);
      X  = _mm_unpacklo_epi32(D8, D12);
      Y  = _mm_unpackhi_epi32(D8, D12);
      E8 = _mm_unpacklo_epi32(X, Y);    // [  8 10 12 14 ]
      O9 = _mm_unpackhi_epi32(X, Y);    // [  9 11 13 15 ]
      O7 = _mm_alignr_epi8(O9, O1, 12); // [  7  9 11 13 ]

      ZE8 = _mm_sub_epi32(E8, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O9, O7), TWO), 2));   // {  8 10 12 14 }
      {
        ZE2 = _mm_alignr_epi8(ZE8, ZE0, 4); // {  2  4  6  8 }
        ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }


        Z0  = _mm_slli_epi32(_mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1), OFFSET), (16 - active_bits)); // {  0  1  2  3 }
        Z4  = _mm_slli_epi32(_mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1), OFFSET), (16 - active_bits)); // {  4  5  6  7 }
        ZZ0 = _mm_srli_epi16(_mm_packus_epi32(Z0, Z4), (16 - active_bits));
        _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x - ooffset_x)*2], ZZ0);
      }
      _mm_prefetch((char *)&idata[y*istride + x + 16], _MM_HINT_T0);

      O1  = O9;
      ZE0 = ZE8;
    }

    if (x < ooffset_x + owidth) {
      ZE2 = _mm_shuffle_epi32(ZE0, 0xF9); // {  2  4  6  6 }
      ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }


      Z0  = _mm_slli_epi32(_mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1), OFFSET), 6); // {  0  1  2  3 }
      Z4  = _mm_slli_epi32(_mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1), OFFSET), 6); // {  4  5  6  7 }
      ZZ0 = _mm_srli_epi16(_mm_packus_epi32(Z0, Z4), 6);
      _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x - ooffset_x)*2], ZZ0);
    }
  }
}

template<int active_bits> void LeGall_5_3_invtransform_H_final_1_10_sse4_2_int16_t(void *_idata,
                                                                     const int istride,
                                                                     const char *odata,
                                                                     const int ostride,
                                                                     const int iwidth,
                                                                     const int iheight,
                                                                     const int ooffset_x,
                                                                     const int ooffset_y,
                                                                     const int owidth,
                                                                     const int oheight){
  int16_t *idata = (int16_t *)_idata;
  const __m128i TWO = _mm_set1_epi16(2);
  const __m128i SHUF = _mm_set_epi8(15,14, 11,10, 7,6, 3,2,
                                    13,12,   9,8, 5,4, 1,0);
  const __m128i CLIP = _mm_set1_epi16((1 << active_bits) - 1);

  const int skip = 1;
  for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y+=skip) {
    int x = (ooffset_x < 16)?0:(ooffset_x - 16);
    __m128i D0, D8, D16, D24, E0, E16, Om1, O1, O15, O17, ZE0, ZE2, ZE16, ZO1, Z0, Z8;

    {
      D0 = _mm_load_si128((__m128i *)&idata[y*istride + x + 0]);
      D8 = _mm_load_si128((__m128i *)&idata[y*istride + x + 8]);
      D0 = _mm_shuffle_epi8(D0, SHUF);
      D8 = _mm_shuffle_epi8(D8, SHUF);
      E0 = _mm_unpacklo_epi64(D0, D8);
      O1 = _mm_unpackhi_epi64(D0, D8);
      Om1 = _mm_slli_si128(O1, 2);
      Om1 = _mm_shufflelo_epi16(Om1, 0xE5);

      ZE0 = _mm_sub_epi16(E0, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(O1, Om1), TWO), 2));
    }

    if (x < ooffset_x) {
      D16 = _mm_load_si128((__m128i *)&idata[y*istride + x + 16]);
      D24 = _mm_load_si128((__m128i *)&idata[y*istride + x + 24]);
      D16 = _mm_shuffle_epi8(D16, SHUF);
      D24 = _mm_shuffle_epi8(D24, SHUF);
      E16 = _mm_unpacklo_epi64(D16, D24);
      O17 = _mm_unpackhi_epi64(D16, D24);
      O15 = _mm_alignr_epi8(O17, O1, 14);

      ZE16 = _mm_sub_epi16(E16, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(O17, O15), TWO), 2));

      O1  = O17;
      ZE0 = ZE16;

      x += 16;
    }

    for (; x < iwidth - 16 && x < ooffset_x + owidth; x += 16) {
      D16 = _mm_load_si128((__m128i *)&idata[y*istride + x + 16]);
      D24 = _mm_load_si128((__m128i *)&idata[y*istride + x + 24]);
      D16 = _mm_shuffle_epi8(D16, SHUF);
      D24 = _mm_shuffle_epi8(D24, SHUF);
      E16 = _mm_unpacklo_epi64(D16, D24);
      O17 = _mm_unpackhi_epi64(D16, D24);
      O15 = _mm_alignr_epi8(O17, O1, 14);

      ZE16 = _mm_sub_epi16(E16, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(O17, O15), TWO), 2));
      {
        __m128i ONE = _mm_srai_epi16(TWO, 1);
        ZE2 = _mm_alignr_epi8(ZE16, ZE0, 2);
        ZO1 = _mm_add_epi16(O1, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(ZE0, ZE2), ONE), 1));

        __m128i ZERO = _mm_srai_epi16(TWO, 2);
        __m128i OFFSET = _mm_slli_epi16(TWO, active_bits - 2);
        Z0  = _mm_max_epi16(_mm_min_epi16(_mm_add_epi16(_mm_srai_epi16(_mm_add_epi16(_mm_unpacklo_epi16(ZE0, ZO1), ONE), 1), OFFSET), CLIP), ZERO);
        Z8  = _mm_max_epi16(_mm_min_epi16(_mm_add_epi16(_mm_srai_epi16(_mm_add_epi16(_mm_unpackhi_epi16(ZE0, ZO1), ONE), 1), OFFSET), CLIP), ZERO);
        _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x + 0 - ooffset_x)*2], Z0);
        _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x + 8 - ooffset_x)*2], Z8);
      }

      O1  = O17;
      ZE0 = ZE16;
    }

    if (x < ooffset_x + owidth) {
      __m128i ONE = _mm_srai_epi16(TWO, 1);
      ZE2 = _mm_srli_si128(ZE0, 2);
      ZE2 = _mm_shufflehi_epi16(ZE2, 0xA4);
      ZO1 = _mm_add_epi16(O1, _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(ZE0, ZE2), ONE), 1));

      __m128i ZERO = _mm_srai_epi16(TWO, 2);
      __m128i OFFSET = _mm_slli_epi16(TWO, active_bits - 2);
      Z0  = _mm_max_epi16(_mm_min_epi16(_mm_add_epi16(_mm_srai_epi16(_mm_add_epi16(_mm_unpacklo_epi16(ZE0, ZO1), ONE), 1), OFFSET), CLIP), ZERO);
      Z8  = _mm_max_epi16(_mm_min_epi16(_mm_add_epi16(_mm_srai_epi16(_mm_add_epi16(_mm_unpackhi_epi16(ZE0, ZO1), ONE), 1), OFFSET), CLIP), ZERO);
      _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x + 0 - ooffset_x)*2], Z0);
      _mm_storeu_si128((__m128i *)&odata[((y - ooffset_y)*ostride + x + 8 - ooffset_x)*2], Z8);
    }
  }
}

void LeGall_5_3_invtransform_H_inplace_2_sse4_2(void *_idata,
                                              const int istride,
                                              const int width,
                                              const int height) {
  int32_t *idata = (int32_t *)_idata;
  const __m128i ONE = _mm_set1_epi32(1);
  const __m128i TWO = _mm_set1_epi32(2);

  const int skip = 2;
  for (int y = 0; y < height; y+=skip) {
    int x = 0;

    __m128i D0, D4, D8, D12, D16, D20, D24, D28, E0, E8, Om1, O1, O9, O7, A, B, C, D, X0, X4, X8, X12, ZE0, ZE2, ZE8, ZO1, W0, W4, Z0, Z4, Z8, Z12;
    D0  = _mm_load_si128((__m128i *)&idata[y*istride + x +  0]); // [  0  A  1  B ]
    D4  = _mm_load_si128((__m128i *)&idata[y*istride + x +  4]); // [  2  C  3  D ]
    D8  = _mm_load_si128((__m128i *)&idata[y*istride + x +  8]); // [  4  E  5  F ]
    D12 = _mm_load_si128((__m128i *)&idata[y*istride + x + 12]); // [  6  G  7  H ]

    A  = _mm_unpacklo_epi32(D0,  D4); // [  0  2  A  C ]
    B  = _mm_unpackhi_epi32(D0,  D4); // [  1  3  B  D ]
    C  = _mm_unpacklo_epi32(D8, D12); // [  4  6  E  G ]
    D  = _mm_unpackhi_epi32(D8, D12); // [  5  7  F  H ]

    E0 = _mm_unpacklo_epi64(A, C);    // [  0  2  4  6 ]
    O1 = _mm_unpacklo_epi64(B, D);    // [  1  3  5  7 ]
    X0 = _mm_unpackhi_epi32(A, B);    // [  A  B  C  D ]
    X4 = _mm_unpackhi_epi32(C, D);    // [  E  F  G  H ]

    Om1 = _mm_shuffle_epi32(O1, 0x90); // [  1  1  3  5 ]
    ZE0 = _mm_sub_epi32(E0, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O1, Om1), TWO), 2)); // {  0  2  4  6 }

    for (; x < width - 16; x += 16) {
      D16 = _mm_load_si128((__m128i *)&idata[y*istride + x + 16]); // [  8  I  9  J ]
      D20 = _mm_load_si128((__m128i *)&idata[y*istride + x + 20]); // [ 10  K 11  L ]
      D24 = _mm_load_si128((__m128i *)&idata[y*istride + x + 24]); // [ 12  M 13  N ]
      D28 = _mm_load_si128((__m128i *)&idata[y*istride + x + 28]); // [ 14  O 15  P ]

      A  = _mm_unpacklo_epi32(D16, D20); // [  8 10  I  K ]
      B  = _mm_unpackhi_epi32(D16, D20); // [  9 11  J  L ]
      C  = _mm_unpacklo_epi32(D24, D28); // [ 12 14  M  O ]
      D  = _mm_unpackhi_epi32(D24, D28); // [ 13 15  N  P ]

      E8  = _mm_unpacklo_epi64(A, C);    // [  8 10 12 14 ]
      O9  = _mm_unpacklo_epi64(B, D);    // [  9 11 13 15 ]
      X8  = _mm_unpackhi_epi32(A, B);    // [  I  J  K  L ]
      X12 = _mm_unpackhi_epi32(C, D);    // [  M  N  O  P ]


      O7 = _mm_alignr_epi8(O9, O1, 12);   // [  7  9 11 13 ]
      ZE8 = _mm_sub_epi32(E8, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(O9, O7), TWO), 2));   // {  8 10 12 14 }
      ZE2 = _mm_alignr_epi8(ZE8, ZE0, 4); // {  2  4  6  8 }
      ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }

      W0 = _mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1); // {  0  1  2  3 }
      Z0 = _mm_unpacklo_epi32(W0, X0); // {  0  A  1  B  }
      Z4 = _mm_unpackhi_epi32(W0, X0); // {  2  C  3  D  }
      _mm_store_si128((__m128i *)&idata[y*istride + x + 0], Z0);
      _mm_store_si128((__m128i *)&idata[y*istride + x + 4], Z4);

      W4 = _mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1); // {  4  5  6  7 }
      Z8  = _mm_unpacklo_epi32(W4, X4); // {  4  E  5  F  }
      Z12 = _mm_unpackhi_epi32(W4, X4); // {  6  G  7  H  }
      _mm_store_si128((__m128i *)&idata[y*istride + x +  8],  Z8);
      _mm_store_si128((__m128i *)&idata[y*istride + x + 12], Z12);

      O1  = O9;
      ZE0 = ZE8;
      X0 = X8;
      X4 = X12;
    }

    ZE2 = _mm_shuffle_epi32(ZE0, 0xF9); // {  2  4  6  6 }
    ZO1 = _mm_add_epi32(O1, _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(ZE0, ZE2), ONE), 1)); // {  1  3  5  7 }

    W0 = _mm_srai_epi32(_mm_add_epi32(_mm_unpacklo_epi32(ZE0, ZO1), ONE), 1); // {  0  1  2  3 }
    Z0 = _mm_unpacklo_epi32(W0, X0); // {  0  A  1  B  }
    Z4 = _mm_unpackhi_epi32(W0, X0); // {  2  C  3  D  }
    _mm_store_si128((__m128i *)&idata[y*istride + x + 0], Z0);
    _mm_store_si128((__m128i *)&idata[y*istride + x + 4], Z4);

    W4 = _mm_srai_epi32(_mm_add_epi32(_mm_unpackhi_epi32(ZE0, ZO1), ONE), 1); // {  4  5  6  7 }
    Z8  = _mm_unpacklo_epi32(W4, X4); // {  4  E  5  F  }
    Z12 = _mm_unpackhi_epi32(W4, X4); // {  6  G  7  H  }
    _mm_store_si128((__m128i *)&idata[y*istride + x +  8],  Z8);
    _mm_store_si128((__m128i *)&idata[y*istride + x + 12], Z12);
  }
}
