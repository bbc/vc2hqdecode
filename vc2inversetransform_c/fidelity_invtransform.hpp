/*****************************************************************************
 * fidelity_invtransform.hpp : Fidelity filter inverse transform functions: 
 *                             plain C++ version
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

#define MIN(A,B) (((A)>(B))?(B):(A))
#define MAX(A,B) (((A)<(B))?(B):(A))

template<int skip>void Fidelity_invtransform_V_inplace(void *_idata,
                                                       const int istride,
                                                       const int width,
                                                       const int height) {
  int32_t *idata = (int32_t *)_idata;
  for (int x = 0; x < width; x += skip) {
    int y = 0;
    int32_t Xe[8]; // -6, -4, -2, +0, +2, +4, +6, +8
    int32_t Xo[8]; // -7, -5, -3, -1, +1, +3, +5, +7
    int32_t Do[8]; // -13, -11, -9, -7, -5, -3, -1, +1
    int32_t Dm6;

    Xe[2] = Xe[3] = idata[(y + 0*skip)*istride + x];
    Xo[3] = Xo[4] = idata[(y + 1*skip)*istride + x];
    Xe[1] = Xe[4] = idata[(y + 2*skip)*istride + x];
    Xo[2] = Xo[5] = idata[(y + 3*skip)*istride + x];
    Xe[0] = Xe[5] = idata[(y + 4*skip)*istride + x];
    Xo[1] = Xo[6] = idata[(y + 5*skip)*istride + x];
    Xe[6] = idata[(y + 6*skip)*istride + x];
    Xo[0] = Xo[7] = idata[(y + 7*skip)*istride + x];
    Xe[7] = idata[(y + 8*skip)*istride + x];

    {
      Do[6] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = idata[(y + 10*skip)*istride + x];

      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    y += 2*skip;

    {
      Do[4] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = idata[(y + 10*skip)*istride + x];

      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    y += 2*skip;

    {
      Do[2] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = idata[(y + 10*skip)*istride + x];

      Do[1] = Do[2];
      Do[2] = Do[3];
      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    y += 2*skip;

    {
      Do[0] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = idata[(y + 10*skip)*istride + x];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    for (; y < height - 10*skip; y += 2*skip) {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = idata[(y + 10*skip)*istride + x];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[(y +  9*skip)*istride + x];
      Xe[7] = Xe[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 6; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xe[6 + i] = Xe[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Xo[i] = Xo[i+1];
      }
      Xo[7] = Xo[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 5; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xe[5 + i] = Xe[4 - i];
      }

      for (int i = 0; i < 6; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xo[6 + i] = Xo[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 4; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xe[4 + i] = Xe[3 - i];
      }

      for (int i = 0; i < 5; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[5 + i] = Xo[4 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 3; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    y += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      for (int i = 0; i < 2; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 6; i++) {
        Do[i] = Do[i+1];
      }
      Do[7] = Do[6];
    }
    y += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];

      Xe[0] = Xe[1];
      for (int i = 0; i < 2; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 5; i++) {
        Do[i] = Do[i+1];
      }
      Do[6] = Do[5];
      Do[7] = Do[4];
    }
    y += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[(y - 6*skip)*istride + x] = Dm6;
      idata[(y - 5*skip)*istride + x] = Xo[1];
    }
  }
}

template<int skip>void Fidelity_invtransform_H_inplace(void *_idata,
                                                                    const int istride,
                                                                    const int width,
                                                                    const int height) {
  int32_t *idata = (int32_t *)_idata;
  for (int y = 0; y < height; y += skip) {
    int x = 0;
    int32_t Xe[8]; // -6, -4, -2, +0, +2, +4, +6, +8
    int32_t Xo[8]; // -7, -5, -3, -1, +1, +3, +5, +7
    int32_t Do[8]; // -13, -11, -9, -7, -5, -3, -1, +1
    int32_t Dm6;

    Xe[2] = Xe[3] = idata[y*istride + x + 0*skip];
    Xo[3] = Xo[4] = idata[y*istride + x + 1*skip];
    Xe[1] = Xe[4] = idata[y*istride + x + 2*skip];
    Xo[2] = Xo[5] = idata[y*istride + x + 3*skip];
    Xe[0] = Xe[5] = idata[y*istride + x + 4*skip];
    Xo[1] = Xo[6] = idata[y*istride + x + 5*skip];
    Xe[6]         = idata[y*istride + x + 6*skip];
    Xo[0] = Xo[7] = idata[y*istride + x + 7*skip];
    Xe[7]         = idata[y*istride + x + 8*skip];

    {
      Do[6] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[4] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[2] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[1] = Do[2];
      Do[2] = Do[3];
      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[0] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    for (; x < width - 10*skip; x += 2*skip) {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = Xe[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 6; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xe[6 + i] = Xe[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Xo[i] = Xo[i+1];
      }
      Xo[7] = Xo[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 5; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xe[5 + i] = Xe[4 - i];
      }

      for (int i = 0; i < 6; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xo[6 + i] = Xo[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 4; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xe[4 + i] = Xe[3 - i];
      }

      for (int i = 0; i < 5; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[5 + i] = Xo[4 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 3; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      for (int i = 0; i < 2; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 6; i++) {
        Do[i] = Do[i+1];
      }
      Do[7] = Do[6];
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;

      Xe[0] = Xe[1];
      for (int i = 0; i < 2; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 5; i++) {
        Do[i] = Do[i+1];
      }
      Do[6] = Do[5];
      Do[7] = Do[4];
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      idata[y*istride + x - 6*skip] = Dm6   >> 1;
      idata[y*istride + x - 5*skip] = Xo[1] >> 1;
    }
  }
}

template<int active_bits> void Fidelity_invtransform_H_final_1(void *_idata,
                                                     const int istride,
                                                     const char *odata,
                                                     const int ostride,
                                                     const int iwidth,
                                                     const int iheight,
                                                     const int ooffset_x,
                                                     const int ooffset_y,
                                                     const int owidth,
                                                     const int oheight) {
  int32_t *idata = (int32_t *)_idata;
  const int skip = 1;
  const uint16_t clip = (1 << active_bits) - 1;
  const uint16_t offset = 1 << (active_bits - 1);

  for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y+=skip) {
    int x = 0;
    int32_t Xe[8]; // -6, -4, -2, +0, +2, +4, +6, +8
    int32_t Xo[8]; // -7, -5, -3, -1, +1, +3, +5, +7
    int32_t Do[8]; // -13, -11, -9, -7, -5, -3, -1, +1
    int32_t Dm6;

    Xe[2] = Xe[3] = idata[y*istride + x + 0*skip];
    Xo[3] = Xo[4] = idata[y*istride + x + 1*skip];
    Xe[1] = Xe[4] = idata[y*istride + x + 2*skip];
    Xo[2] = Xo[5] = idata[y*istride + x + 3*skip];
    Xe[0] = Xe[5] = idata[y*istride + x + 4*skip];
    Xo[1] = Xo[6] = idata[y*istride + x + 5*skip];
    Xe[6]         = idata[y*istride + x + 6*skip];
    Xo[0] = Xo[7] = idata[y*istride + x + 7*skip];
    Xe[7]         = idata[y*istride + x + 8*skip];

    {
      Do[6] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[4] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[2] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      Do[1] = Do[2];
      Do[2] = Do[3];
      Do[3] = Do[4];
      Do[4] = Do[5];
      Do[5] = Do[6];
      Do[6] = Do[7];
    }
    x += 2*skip;

    {
      Do[0] = Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                                +10*(Xe[1] + Xe[6])
                                -25*(Xe[2] + Xe[5])
                                +81*(Xe[3] + Xe[4])
                                + 128) >> 8);

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    for (; x < iwidth - 10*skip; x += 2*skip) {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = idata[y*istride + x + 10*skip];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 7; i++) {
        Xe[i] = Xe[i+1];
        Xo[i] = Xo[i+1];
      }
      Xo[7] = idata[y*istride + x +  9*skip];
      Xe[7] = Xe[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 6; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xe[6 + i] = Xe[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Xo[i] = Xo[i+1];
      }
      Xo[7] = Xo[6];

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 5; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xe[5 + i] = Xe[4 - i];
      }

      for (int i = 0; i < 6; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 2; i++) {
        Xo[6 + i] = Xo[5 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 4; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xe[4 + i] = Xe[3 - i];
      }

      for (int i = 0; i < 5; i++) {
        Xo[i] = Xo[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[5 + i] = Xo[4 - i];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Do[7] = Xo[4] + ((-2*( Xe[0] + Xe[7])
                        +10*(Xe[1] + Xe[6])
                        -25*(Xe[2] + Xe[5])
                        +81*(Xe[3] + Xe[4])
                        + 128) >> 8);
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 3; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 4; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 7; i++) {
        Do[i] = Do[i+1];
      }
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      for (int i = 0; i < 2; i++) {
        Xe[i] = Xe[i+1];
      }
      for (int i = 0; i < 3; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 6; i++) {
        Do[i] = Do[i+1];
      }
      Do[7] = Do[6];
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }

      Xe[0] = Xe[1];
      for (int i = 0; i < 2; i++) {
        Xo[i] = Xo[i+1];
      }

      for (int i = 0; i < 5; i++) {
        Do[i] = Do[i+1];
      }
      Do[6] = Do[5];
      Do[7] = Do[4];
    }
    x += 2*skip;

    {
      Dm6 = Xe[0] - ((-8*(  Do[0] + Do[7])
                      +21*( Do[1] + Do[6])
                      -46*( Do[2] + Do[5])
                      +161*(Do[3] + Do[4]) + 128) >> 8);

      if (x >= ooffset_x + 6*skip && x < ooffset_x + owidth + 5*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 6*skip] = (uint16_t)MIN(MAX(((Dm6   >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Xo[1] >> 1) + offset), 0), clip);
      }
    }
  }
}
