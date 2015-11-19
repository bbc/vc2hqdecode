/*****************************************************************************
 * dequantise_sse4_2.cpp : dequantiser functions: SSE4.2 version
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
#include "../vc2inversetransform_c/dequantise_c.hpp"
#include "dequantise_sse4_2.hpp"

#define sgn(x) (((x)==0)?0:(((x)>0)?1:-1))

template <int slice_width,
          int slice_height,
          int depth,
          class T> void dequantise_sse4_2(QuantisationMatrix *qmatrix,
                                          int32_t *idata,
                                          void *_odata,
                                          int ostride,
                                          int, int, int);

inline __m128i LOAD_QUANTISED(const int32_t *idata, const QuantisationMatrix *qmatrix, const int l, const int s) {
  __m128i D  = _mm_load_si128((__m128i *)idata);
  __m128i QF = _mm_load_si128((__m128i *)&qmatrix->qfactor[l][s]);
  __m128i QO = _mm_load_si128((__m128i *)&qmatrix->qoffset[l][s]);
  __m128i X  = _mm_abs_epi32(D);
  X = _mm_mullo_epi32(X, QF);
  X = _mm_add_epi32(X, QO);
  X = _mm_srai_epi32(X, 2);
  X = _mm_sign_epi32(X, D);
  return X;
}

template<class T> inline void STORE_SAMPLE_PAIR(__m128i *tgt, __m128i A, __m128i B);

template<> inline void STORE_SAMPLE_PAIR<int32_t>(__m128i *tgt, __m128i A, __m128i B) {
  _mm_store_si128(tgt + 0, A);
  _mm_store_si128(tgt + 1, B);
}

template<> inline void STORE_SAMPLE_PAIR<int16_t>(__m128i *tgt, __m128i A, __m128i B) {
  __m128i X = _mm_packs_epi32(A, B);
  _mm_store_si128(tgt + 0, X);
}

template<class T> inline void dequantise_sse4_2_8_8_2(QuantisationMatrix *qmatrix,
                                                      int32_t *idata,
                                                      void *_odata,
                                                      int ostride) {
  T *odata = (T *)_odata;
  const int slice_width  = 8;
  const int slice_height = 8;
  const int Y = 0;
  const int X = 0;
  const int N = 0;
  T * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  const __m128i D0  = LOAD_QUANTISED(&iptr[ 0], qmatrix, 0, 0); // [  0  1  2  3 ]
  const __m128i D4  = LOAD_QUANTISED(&iptr[ 4], qmatrix, 1, 1); // [  4  5  6  7 ]
  const __m128i D8  = LOAD_QUANTISED(&iptr[ 8], qmatrix, 1, 2); // [  8  9 10 11 ]
  const __m128i D12 = LOAD_QUANTISED(&iptr[12], qmatrix, 1, 3); // [ 12 13 14 15 ]
  const __m128i D16 = LOAD_QUANTISED(&iptr[16], qmatrix, 2, 1); // [ 16 17 18 19 ]
  const __m128i D20 = LOAD_QUANTISED(&iptr[20], qmatrix, 2, 1); // [ 20 21 22 23 ]
  const __m128i D24 = LOAD_QUANTISED(&iptr[24], qmatrix, 2, 1); // [ 24 25 26 27 ]
  const __m128i D28 = LOAD_QUANTISED(&iptr[28], qmatrix, 2, 1); // [ 28 29 30 31 ]

  const __m128i X0 = _mm_unpacklo_epi32(D0,  D4); // [  0  4  1  5 ]
  const __m128i Y0 = _mm_unpacklo_epi32(X0, D16); // [  0 16  4 17 ]
  const __m128i Y1 = _mm_unpackhi_epi32(X0, D16); // [  1 18  5 19 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride + 0], Y0, Y1);

  const __m128i X1 = _mm_unpackhi_epi32(D0,  D4); // [  2  6  3  7 ]
  const __m128i Y2 = _mm_unpacklo_epi32(X1, D24); // [  2 24  6 25 ]
  const __m128i Y3 = _mm_unpackhi_epi32(X1, D24); // [  3 26  7 27 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride + 0], Y2, Y3);


  const __m128i X2 = _mm_unpacklo_epi32(D8, D12);  // [  8 12  9 13 ]
  const __m128i Y4 = _mm_unpacklo_epi32(X2, D20);  // [  8 20 12 21 ]
  const __m128i Y5 = _mm_unpackhi_epi32(X2, D20);  // [  9 22 13 23 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride + 0], Y4, Y5);

  const __m128i X3 = _mm_unpackhi_epi32(D8, D12);  // [ 10 14 11 15 ]
  const __m128i Y6 = _mm_unpacklo_epi32(X3, D28);  // [ 10 28 14 29 ]
  const __m128i Y7 = _mm_unpackhi_epi32(X3, D28);  // [ 11 30 15 31 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride + 0], Y6, Y7);



  const __m128i D32 = LOAD_QUANTISED(&iptr[32], qmatrix, 2, 2); // [ 32 33 34 35 ]
  const __m128i D36 = LOAD_QUANTISED(&iptr[36], qmatrix, 2, 2); // [ 36 37 38 39 ]
  const __m128i D40 = LOAD_QUANTISED(&iptr[40], qmatrix, 2, 2); // [ 40 41 42 43 ]
  const __m128i D44 = LOAD_QUANTISED(&iptr[44], qmatrix, 2, 2); // [ 44 45 46 47 ]
  const __m128i D48 = LOAD_QUANTISED(&iptr[48], qmatrix, 2, 3); // [ 48 49 50 51 ]
  const __m128i D52 = LOAD_QUANTISED(&iptr[52], qmatrix, 2, 3); // [ 52 53 54 55 ]
  const __m128i D56 = LOAD_QUANTISED(&iptr[56], qmatrix, 2, 3); // [ 56 57 58 59 ]
  const __m128i D60 = LOAD_QUANTISED(&iptr[60], qmatrix, 2, 3); // [ 60 61 62 63 ]

  const __m128i Z0 = _mm_unpacklo_epi32(D32, D48); // [ 32 48 33 49 ]
  const __m128i Z1 = _mm_unpackhi_epi32(D32, D48); // [ 34 50 35 51 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[1*ostride + 0], Z0, Z1);

  const __m128i Z2 = _mm_unpacklo_epi32(D36, D52); // [ 36 52 37 53 ]
  const __m128i Z3 = _mm_unpackhi_epi32(D36, D52); // [ 38 54 39 55 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[3*ostride + 0], Z2, Z3);

  const __m128i Z4 = _mm_unpacklo_epi32(D40, D56); // [ 40 56 41 57 ]
  const __m128i Z5 = _mm_unpackhi_epi32(D40, D56); // [ 42 58 43 59 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[5*ostride + 0], Z4, Z5);

  const __m128i Z6 = _mm_unpacklo_epi32(D44, D60); // [ 44 60 45 61 ]
  const __m128i Z7 = _mm_unpackhi_epi32(D44, D60); // [ 46 62 47 63 ]
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[7*ostride + 0], Z6, Z7);
}

template<> void dequantise_sse4_2<4,8,2, int32_t>(QuantisationMatrix *qmatrix,
                                  int32_t *idata,
                                  void *_odata,
                                  int ostride,
                                  int, int, int) {
  int32_t *odata = (int32_t *)_odata;
  const int slice_width  = 4;
  const int slice_height = 8;
  const int Y = 0;
  const int X = 0;
  const int N = 0;

  int32_t * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  __m128i D0;
  {
    D0 = _mm_load_si128((__m128i *)&iptr[ 0]); // [  0  1  2  3 ] (Q)
    __m128i QF = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qfactor[0][0]),
                                    _mm_load_si128((__m128i *)&qmatrix->qfactor[1][1]));
    __m128i QO = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qoffset[0][0]),
                                    _mm_load_si128((__m128i *)&qmatrix->qoffset[1][1]));

    __m128i X  = _mm_abs_epi32(D0);
    X = _mm_mullo_epi32(X, QF);
    X = _mm_add_epi32(X, QO);
    X = _mm_srai_epi32(X, 2);
    D0 = _mm_sign_epi32(X, D0);
  }
  __m128i D4;
  {
    D4 = _mm_load_si128((__m128i *)&iptr[ 4]); // [  4  5  6  7 ] (Q)
    __m128i QF = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qfactor[1][2]),
                                    _mm_load_si128((__m128i *)&qmatrix->qfactor[1][3]));
    __m128i QO = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qoffset[1][2]),
                                    _mm_load_si128((__m128i *)&qmatrix->qoffset[1][3]));
    __m128i X  = _mm_abs_epi32(D4);
    X = _mm_mullo_epi32(X, QF);
    X = _mm_add_epi32(X, QO);
    X = _mm_srai_epi32(X, 2);
    D4 = _mm_sign_epi32(X, D4);
  }

  const __m128i D8  = LOAD_QUANTISED(&iptr[ 8], qmatrix, 2, 1); // [  8  9 10 11 ]
  const __m128i D12 = LOAD_QUANTISED(&iptr[12], qmatrix, 2, 1); // [ 12 13 14 15 ]
  const __m128i D16 = LOAD_QUANTISED(&iptr[16], qmatrix, 2, 2); // [ 16 17 18 19 ]
  const __m128i D20 = LOAD_QUANTISED(&iptr[20], qmatrix, 2, 2); // [ 20 21 22 23 ]
  const __m128i D24 = LOAD_QUANTISED(&iptr[24], qmatrix, 2, 3); // [ 24 25 26 27 ]
  const __m128i D28 = LOAD_QUANTISED(&iptr[28], qmatrix, 2, 3); // [ 28 29 30 31 ]

  const __m128i X0  = _mm_unpacklo_epi32(D0,  D4); // [  0  4  1  5 ]
  const __m128i X1  = _mm_unpackhi_epi32(D0,  D4); // [  2  6  3  7 ]
  const __m128i Y0  = _mm_unpacklo_epi32(X0,  X1); // [  0  2  4  6 ]
  const __m128i Y1  = _mm_unpackhi_epi32(X0,  X1); // [  1  3  5  7 ]

  const __m128i Z0  = _mm_unpacklo_epi32(Y0,  D8); // [  0  8  2  9 ]
  _mm_store_si128((__m128i *)&optr[0*ostride + 0], Z0);
  const __m128i Z1  = _mm_unpackhi_epi32(Y0,  D8); // [  4 10  6 11 ]
  _mm_store_si128((__m128i *)&optr[2*ostride + 0], Z1);

  const __m128i Z2  = _mm_unpacklo_epi32(Y1, D12); // [  1 12  3 13 ]
  _mm_store_si128((__m128i *)&optr[4*ostride + 0], Z2);
  const __m128i Z3  = _mm_unpackhi_epi32(Y1, D12); // [  5 14  7 15 ]
  _mm_store_si128((__m128i *)&optr[6*ostride + 0], Z3);

  const __m128i W0  = _mm_unpacklo_epi32(D16, D24);// [ 16 24 17 25 ]
  _mm_store_si128((__m128i *)&optr[1*ostride + 0], W0);
  const __m128i W1  = _mm_unpackhi_epi32(D16, D24);// [ 18 26 19 27 ]
  _mm_store_si128((__m128i *)&optr[3*ostride + 0], W1);

  const __m128i W2  = _mm_unpacklo_epi32(D20, D28);// [ 20 28 21 29 ]
  _mm_store_si128((__m128i *)&optr[5*ostride + 0], W2);
  const __m128i W3  = _mm_unpackhi_epi32(D20, D28);// [ 22 30 23 31 ]
  _mm_store_si128((__m128i *)&optr[7*ostride + 0], W3);

}

template<class T> inline void dequantise_sse4_2_32_8_3(QuantisationMatrix *qmatrix,
                                                       int32_t *idata,
                                                       void *_odata,
                                                       int ostride) {
  T *odata = (T *)_odata;
  const int slice_width  = 32;
  const int slice_height = 8;

  const int Y = 0;
  const int X = 0;
  const int N = 0;

  T * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  const __m128i D0  = LOAD_QUANTISED(&iptr[ 0], qmatrix, 0, 0);
  const __m128i D4  = LOAD_QUANTISED(&iptr[ 4], qmatrix, 1, 1);

  const __m128i D16 = LOAD_QUANTISED(&iptr[16], qmatrix, 2, 1);
  const __m128i D20 = LOAD_QUANTISED(&iptr[20], qmatrix, 2, 1);

  const __m128i D64 = LOAD_QUANTISED(&iptr[64], qmatrix, 3, 1);
  const __m128i D68 = LOAD_QUANTISED(&iptr[68], qmatrix, 3, 1);
  const __m128i D72 = LOAD_QUANTISED(&iptr[72], qmatrix, 3, 1);
  const __m128i D76 = LOAD_QUANTISED(&iptr[76], qmatrix, 3, 1);

  const __m128i A0  = _mm_unpacklo_epi32(D0, D4);   // ( 00 11 00 11 )
  const __m128i A1  = _mm_unpackhi_epi32(D0, D4);   // ( 00 11 00 11 )

  const __m128i B0  = _mm_unpacklo_epi32(A0, D16);  // ( 00 21 11 21 )
  const __m128i B1  = _mm_unpackhi_epi32(A0, D16);  // ( 00 21 11 21 )
  const __m128i B2  = _mm_unpacklo_epi32(A1, D20);  // ( 00 21 11 21 )
  const __m128i B3  = _mm_unpackhi_epi32(A1, D20);  // ( 00 21 11 21 )

  const __m128i C0  = _mm_unpacklo_epi32(B0, D64);  // ( 00 31 21 31 )
  const __m128i C1  = _mm_unpackhi_epi32(B0, D64);  // ( 11 31 21 31 )
  const __m128i C2  = _mm_unpacklo_epi32(B1, D68);  // ( 00 31 21 31 )
  const __m128i C3  = _mm_unpackhi_epi32(B1, D68);  // ( 11 31 21 31 )
  const __m128i C4  = _mm_unpacklo_epi32(B2, D72);  // ( 00 31 21 31 )
  const __m128i C5  = _mm_unpackhi_epi32(B2, D72);  // ( 11 31 21 31 )
  const __m128i C6  = _mm_unpacklo_epi32(B3, D76);  // ( 00 31 21 31 )
  const __m128i C7  = _mm_unpackhi_epi32(B3, D76);  // ( 11 31 21 31 )

  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride + 0],  C0, C1);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride + 8],  C2, C3);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride + 16], C4, C5);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride + 24], C6, C7);



  const __m128i D8  = LOAD_QUANTISED(&iptr[ 8], qmatrix, 1, 2);
  const __m128i D12 = LOAD_QUANTISED(&iptr[12], qmatrix, 1, 3);

  const __m128i D24 = LOAD_QUANTISED(&iptr[24], qmatrix, 2, 1);
  const __m128i D28 = LOAD_QUANTISED(&iptr[28], qmatrix, 2, 1);

  const __m128i D96  = LOAD_QUANTISED(&iptr[96], qmatrix, 3, 1);
  const __m128i D100 = LOAD_QUANTISED(&iptr[100], qmatrix, 3, 1);
  const __m128i D104 = LOAD_QUANTISED(&iptr[104], qmatrix, 3, 1);
  const __m128i D108 = LOAD_QUANTISED(&iptr[108], qmatrix, 3, 1);

  const __m128i A2  = _mm_unpacklo_epi32(D8, D12);  // ( 12 13 12 13 )
  const __m128i A3  = _mm_unpackhi_epi32(D8, D12);  // ( 12 13 12 13 )

  const __m128i B4  = _mm_unpacklo_epi32(A2,  D24); // ( 12 21 13 21 )
  const __m128i B5  = _mm_unpackhi_epi32(A2,  D24); // ( 12 21 13 21 )
  const __m128i B6  = _mm_unpacklo_epi32(A3,  D28); // ( 12 21 13 21 )
  const __m128i B7  = _mm_unpackhi_epi32(A3,  D28); // ( 12 21 13 21 )

  const __m128i C8  = _mm_unpacklo_epi32(B4,  D96); // ( 12 31 21 31 )
  const __m128i C9  = _mm_unpackhi_epi32(B4,  D96); // ( 13 31 21 31 )
  const __m128i C10 = _mm_unpacklo_epi32(B5,  D100); // ( 12 31 21 31 )
  const __m128i C11 = _mm_unpackhi_epi32(B5,  D100); // ( 13 31 21 31 )
  const __m128i C12 = _mm_unpacklo_epi32(B6,  D104); // ( 12 31 21 31 )
  const __m128i C13 = _mm_unpackhi_epi32(B6,  D104); // ( 13 31 21 31 )
  const __m128i C14 = _mm_unpacklo_epi32(B7,  D108); // ( 12 31 21 31 )
  const __m128i C15 = _mm_unpackhi_epi32(B7,  D108); // ( 13 31 21 31 )

  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride + 0],  C8,  C9);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride + 8],  C10, C11);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride + 16], C12, C13);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride + 24], C14, C15);



  const __m128i D32  = LOAD_QUANTISED(&iptr[ 32], qmatrix, 2, 2);
  const __m128i D36  = LOAD_QUANTISED(&iptr[ 36], qmatrix, 2, 2);

  const __m128i D48  = LOAD_QUANTISED(&iptr[ 48], qmatrix, 2, 3);
  const __m128i D52  = LOAD_QUANTISED(&iptr[ 52], qmatrix, 2, 3);

  const __m128i D80 = LOAD_QUANTISED(&iptr[ 80], qmatrix, 3, 1);
  const __m128i D84 = LOAD_QUANTISED(&iptr[ 84], qmatrix, 3, 1);
  const __m128i D88 = LOAD_QUANTISED(&iptr[ 88], qmatrix, 3, 1);
  const __m128i D92 = LOAD_QUANTISED(&iptr[ 92], qmatrix, 3, 1);

  const __m128i A4  = _mm_unpacklo_epi32(D32, D48);  // ( 22 23 22 23 )
  const __m128i A5  = _mm_unpackhi_epi32(D32, D48);  // ( 22 23 22 23 )
  const __m128i A6  = _mm_unpacklo_epi32(D36, D52);  // ( 22 23 22 23 )
  const __m128i A7  = _mm_unpackhi_epi32(D36, D52);  // ( 22 23 22 23 )

  const __m128i B8  = _mm_unpacklo_epi32(A4,  D80);  // ( 22 31 23 31 )
  const __m128i B9  = _mm_unpackhi_epi32(A4,  D80);  // ( 22 31 23 31 )
  const __m128i B10 = _mm_unpacklo_epi32(A5,  D84); // ( 22 31 23 31 )
  const __m128i B11 = _mm_unpackhi_epi32(A5,  D84); // ( 22 31 23 31 )
  const __m128i B12 = _mm_unpacklo_epi32(A6,  D88); // ( 22 31 23 31 )
  const __m128i B13 = _mm_unpackhi_epi32(A6,  D88); // ( 22 31 23 31 )
  const __m128i B14 = _mm_unpacklo_epi32(A7,  D92); // ( 22 31 23 31 )
  const __m128i B15 = _mm_unpackhi_epi32(A7,  D92); // ( 22 31 23 31 )

  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride + 0],  B8,  B9);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride + 8],  B10, B11);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride + 16], B12, B13);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride + 24], B14, B15);



  const __m128i D40  = LOAD_QUANTISED(&iptr[ 40], qmatrix, 2, 2);
  const __m128i D44  = LOAD_QUANTISED(&iptr[ 44], qmatrix, 2, 2);

  const __m128i D56  = LOAD_QUANTISED(&iptr[ 56], qmatrix, 2, 3);
  const __m128i D60  = LOAD_QUANTISED(&iptr[ 60], qmatrix, 2, 3);

  const __m128i D112 = LOAD_QUANTISED(&iptr[112], qmatrix, 3, 1);
  const __m128i D116 = LOAD_QUANTISED(&iptr[116], qmatrix, 3, 1);
  const __m128i D120 = LOAD_QUANTISED(&iptr[120], qmatrix, 3, 1);
  const __m128i D124 = LOAD_QUANTISED(&iptr[124], qmatrix, 3, 1);

  const __m128i A8  = _mm_unpacklo_epi32(D40, D56);  // ( 22 23 22 23 )
  const __m128i A9  = _mm_unpackhi_epi32(D40, D56);  // ( 22 23 22 23 )
  const __m128i A10 = _mm_unpacklo_epi32(D44, D60);  // ( 22 23 22 23 )
  const __m128i A11 = _mm_unpackhi_epi32(D44, D60);  // ( 22 23 22 23 )

  const __m128i B16 = _mm_unpacklo_epi32(A8,  D112); // ( 22 31 23 31 )
  const __m128i B17 = _mm_unpackhi_epi32(A8,  D112); // ( 22 31 23 31 )
  const __m128i B18 = _mm_unpacklo_epi32(A9,  D116); // ( 22 31 23 31 )
  const __m128i B19 = _mm_unpackhi_epi32(A9,  D116); // ( 22 31 23 31 )
  const __m128i B20 = _mm_unpacklo_epi32(A10, D120); // ( 22 31 23 31 )
  const __m128i B21 = _mm_unpackhi_epi32(A10, D120); // ( 22 31 23 31 )
  const __m128i B22 = _mm_unpacklo_epi32(A11, D124); // ( 22 31 23 31 )
  const __m128i B23 = _mm_unpackhi_epi32(A11, D124); // ( 22 31 23 31 )

  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride + 0],  B16, B17);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride + 8],  B18, B19);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride + 16], B20, B21);
  STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride + 24], B22, B23);


  for (int j = 0; j < 4; j++) {
    const __m128i X0 = LOAD_QUANTISED(&iptr[128 + j*16 +  0], qmatrix, 3, 2);
    const __m128i X1 = LOAD_QUANTISED(&iptr[128 + j*16 +  4], qmatrix, 3, 2);
    const __m128i X2 = LOAD_QUANTISED(&iptr[128 + j*16 +  8], qmatrix, 3, 2);
    const __m128i X3 = LOAD_QUANTISED(&iptr[128 + j*16 + 12], qmatrix, 3, 2);

    const __m128i Y0 = LOAD_QUANTISED(&iptr[192 + j*16 +  0], qmatrix, 3, 3);
    const __m128i Y1 = LOAD_QUANTISED(&iptr[192 + j*16 +  4], qmatrix, 3, 3);
    const __m128i Y2 = LOAD_QUANTISED(&iptr[192 + j*16 +  8], qmatrix, 3, 3);
    const __m128i Y3 = LOAD_QUANTISED(&iptr[192 + j*16 + 12], qmatrix, 3, 3);

    const __m128i Z0 = _mm_unpacklo_epi32(X0, Y0);
    const __m128i Z1 = _mm_unpackhi_epi32(X0, Y0);
    const __m128i Z2 = _mm_unpacklo_epi32(X1, Y1);
    const __m128i Z3 = _mm_unpackhi_epi32(X1, Y1);
    const __m128i Z4 = _mm_unpacklo_epi32(X2, Y2);
    const __m128i Z5 = _mm_unpackhi_epi32(X2, Y2);
    const __m128i Z6 = _mm_unpacklo_epi32(X3, Y3);
    const __m128i Z7 = _mm_unpackhi_epi32(X3, Y3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*j + 1)*ostride + 0],  Z0, Z1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*j + 1)*ostride + 8],  Z2, Z3);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*j + 1)*ostride + 16], Z4, Z5);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*j + 1)*ostride + 24], Z6, Z7);
  }
}

template<class T> inline void dequantise_sse4_2_16_8_3(QuantisationMatrix *qmatrix,
                                                        int32_t *idata,
                                                        void *_odata,
                                                        int ostride) {
  T *odata = (T *)_odata;
  const int slice_width  = 16;
  const int slice_height = 8;

  const int Y = 0;
  const int X = 0;
  const int N = 0;

  T * const optr = &odata[Y*slice_height*ostride + X*slice_width];
  const int32_t * iptr = &idata[N*slice_height*slice_width];

  {
    __m128i D0;
    {
      D0 = _mm_load_si128((__m128i *)&iptr[ 0]); // [  0  1  2  3 ] (Q)
      __m128i QF = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qfactor[0][0]),
                                      _mm_load_si128((__m128i *)&qmatrix->qfactor[1][1]));
      __m128i QO = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qoffset[0][0]),
                                      _mm_load_si128((__m128i *)&qmatrix->qoffset[1][1]));

      __m128i X  = _mm_abs_epi32(D0);
      X = _mm_mullo_epi32(X, QF);
      X = _mm_add_epi32(X, QO);
      X = _mm_srai_epi32(X, 2);
      D0 = _mm_sign_epi32(X, D0);

      D0 = _mm_shuffle_epi32(D0, 0xD8);
    }

    const __m128i D1 = LOAD_QUANTISED(&iptr[8], qmatrix, 2, 1);

    const __m128i D2 = LOAD_QUANTISED(&iptr[32], qmatrix, 3, 1);
    const __m128i D3 = LOAD_QUANTISED(&iptr[36], qmatrix, 3, 1);

    const __m128i A0  = _mm_unpacklo_epi32(D0, D1);
    const __m128i A1  = _mm_unpackhi_epi32(D0, D1);

    const __m128i B0  = _mm_unpacklo_epi32(A0, D2);
    const __m128i B1  = _mm_unpackhi_epi32(A0, D2);
    const __m128i B2  = _mm_unpacklo_epi32(A1, D3);
    const __m128i B3  = _mm_unpackhi_epi32(A1, D3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride +  0], B0, B1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[0*ostride +  8], B2, B3);
  }

  {
    __m128i D0;
    {
      D0 = _mm_load_si128((__m128i *)&iptr[ 4]);
      __m128i QF = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qfactor[1][2]),
                                      _mm_load_si128((__m128i *)&qmatrix->qfactor[1][3]));
      __m128i QO = _mm_unpacklo_epi64(_mm_load_si128((__m128i *)&qmatrix->qoffset[1][2]),
                                      _mm_load_si128((__m128i *)&qmatrix->qoffset[1][3]));

      __m128i X  = _mm_abs_epi32(D0);
      X = _mm_mullo_epi32(X, QF);
      X = _mm_add_epi32(X, QO);
      X = _mm_srai_epi32(X, 2);
      D0 = _mm_sign_epi32(X, D0);

      D0 = _mm_shuffle_epi32(D0, 0xD8);
    }

    const __m128i D1 = LOAD_QUANTISED(&iptr[12], qmatrix, 2, 1);

    const __m128i D2 = LOAD_QUANTISED(&iptr[48], qmatrix, 3, 1);
    const __m128i D3 = LOAD_QUANTISED(&iptr[52], qmatrix, 3, 1);

    const __m128i A0  = _mm_unpacklo_epi32(D0, D1);
    const __m128i A1  = _mm_unpackhi_epi32(D0, D1);

    const __m128i B0  = _mm_unpacklo_epi32(A0, D2);
    const __m128i B1  = _mm_unpackhi_epi32(A0, D2);
    const __m128i B2  = _mm_unpacklo_epi32(A1, D3);
    const __m128i B3  = _mm_unpackhi_epi32(A1, D3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride +  0], B0, B1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[4*ostride +  8], B2, B3);
  }

  {
    const __m128i D0 = LOAD_QUANTISED(&iptr[16], qmatrix, 2, 2);

    const __m128i D1 = LOAD_QUANTISED(&iptr[24], qmatrix, 2, 3);

    const __m128i D2 = LOAD_QUANTISED(&iptr[40], qmatrix, 3, 1);
    const __m128i D3 = LOAD_QUANTISED(&iptr[44], qmatrix, 3, 1);

    const __m128i A0  = _mm_unpacklo_epi32(D0, D1);
    const __m128i A1  = _mm_unpackhi_epi32(D0, D1);

    const __m128i B0  = _mm_unpacklo_epi32(A0, D2);
    const __m128i B1  = _mm_unpackhi_epi32(A0, D2);
    const __m128i B2  = _mm_unpacklo_epi32(A1, D3);
    const __m128i B3  = _mm_unpackhi_epi32(A1, D3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride +  0], B0, B1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[2*ostride +  8], B2, B3);
  }

  {
    const __m128i D0 = LOAD_QUANTISED(&iptr[20], qmatrix, 2, 2);

    const __m128i D1 = LOAD_QUANTISED(&iptr[28], qmatrix, 2, 3);

    const __m128i D2 = LOAD_QUANTISED(&iptr[56], qmatrix, 3, 1);
    const __m128i D3 = LOAD_QUANTISED(&iptr[60], qmatrix, 3, 1);

    const __m128i A0  = _mm_unpacklo_epi32(D0, D1);
    const __m128i A1  = _mm_unpackhi_epi32(D0, D1);

    const __m128i B0  = _mm_unpacklo_epi32(A0, D2);
    const __m128i B1  = _mm_unpackhi_epi32(A0, D2);
    const __m128i B2  = _mm_unpacklo_epi32(A1, D3);
    const __m128i B3  = _mm_unpackhi_epi32(A1, D3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride +  0], B0, B1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[6*ostride +  8], B2, B3);
  }

  for (int y = 0; y < 4; y++) {
    const __m128i D0 = LOAD_QUANTISED(&iptr[ 64 + y*8], qmatrix, 3, 2);
    const __m128i D1 = LOAD_QUANTISED(&iptr[ 68 + y*8], qmatrix, 3, 2);

    const __m128i D2 = LOAD_QUANTISED(&iptr[ 96 + y*8], qmatrix, 3, 3);
    const __m128i D3 = LOAD_QUANTISED(&iptr[100 + y*8], qmatrix, 3, 3);

    const __m128i A0  = _mm_unpacklo_epi32(D0, D2);
    const __m128i A1  = _mm_unpackhi_epi32(D0, D2);
    const __m128i A2  = _mm_unpacklo_epi32(D1, D3);
    const __m128i A3  = _mm_unpackhi_epi32(D1, D3);

    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*y + 1)*ostride +  0], A0, A1);
    STORE_SAMPLE_PAIR<T>((__m128i *)&optr[(2*y + 1)*ostride +  8], A2, A3);
  }
}

template <> inline void dequantise_sse4_2<16,8,3, int32_t>(QuantisationMatrix *qmatrix,
                                                           int32_t *idata,
                                                           void *_odata,
                                                           int ostride,
                                                           int, int, int) {
  dequantise_sse4_2_16_8_3<int32_t>(qmatrix, idata, _odata, ostride);
}

template <> inline void dequantise_sse4_2<16,8,3, int16_t>(QuantisationMatrix *qmatrix,
                                                           int32_t *idata,
                                                           void *_odata,
                                                           int ostride,
                                                           int, int, int) {
  dequantise_sse4_2_16_8_3<int16_t>(qmatrix, idata, _odata, ostride);
}

template <> inline void dequantise_sse4_2<32,8,3, int32_t>(QuantisationMatrix *qmatrix,
                                                           int32_t *idata,
                                                           void *_odata,
                                                           int ostride,
                                                           int, int, int) {
  dequantise_sse4_2_32_8_3<int32_t>(qmatrix, idata, _odata, ostride);
}

template <> inline void dequantise_sse4_2<32,8,3, int16_t>(QuantisationMatrix *qmatrix,
                                                           int32_t *idata,
                                                           void *_odata,
                                                           int ostride,
                                                           int, int, int) {
  dequantise_sse4_2_32_8_3<int16_t>(qmatrix, idata, _odata, ostride);
}

template<> inline void dequantise_sse4_2<8,8,2, int32_t>(QuantisationMatrix *qmatrix,
                                                         int32_t *idata,
                                                         void *_odata,
                                                         int ostride,
                                                         int, int, int) {
  dequantise_sse4_2_8_8_2<int32_t>(qmatrix, idata, _odata, ostride);
}

template<> inline void dequantise_sse4_2<8,8,2, int16_t>(QuantisationMatrix *qmatrix,
                                                         int32_t *idata,
                                                         void *_odata,
                                                         int ostride,
                                                         int, int, int) {
  dequantise_sse4_2_8_8_2<int16_t>(qmatrix, idata, _odata, ostride);
}

DequantiseFunction getDequantiseFunction_sse4_2(int slice_width,
                                                int slice_height,
                                                int depth,
                                                int sample_size) {
  if (sample_size == 4) {
    if (depth == 2 &&
        slice_width == 8 &&
        slice_height == 8)
      return dequantise_sse4_2<8,8,2, int32_t>;
    else if (depth == 2 &&
             slice_width == 4 &&
             slice_height == 8)
      return dequantise_sse4_2<4,8,2, int32_t>;
    else if (depth == 3 &&
             slice_width == 32 &&
             slice_height == 8)
      return dequantise_sse4_2<32,8,3, int32_t>;
    else if (depth == 3 &&
             slice_width == 16 &&
             slice_height == 8)
      return dequantise_sse4_2<16,8,3, int32_t>;
  } else if (sample_size == 2) {
    /* if (depth == 2 &&
        slice_width == 8 &&
        slice_height == 8)
      return dequantise_sse4_2<8,8,2, int16_t>;
    else if (depth == 2 &&
             slice_width == 4 &&
             slice_height == 8)
      return dequantise_sse4_2<4,8,2, int16_t>;
      else*/ if (depth == 3 &&
             slice_width == 32 &&
             slice_height == 8)
      return dequantise_sse4_2<32,8,3, int16_t>;
      else if (depth == 3 &&
             slice_width == 16 &&
             slice_height == 8)
      return dequantise_sse4_2<16,8,3, int16_t>;
      }

  return getDequantiseFunction_c(slice_width, slice_height, depth, sample_size);
}
