/*****************************************************************************
 * legall_invtransform.hpp : LeGall filter inverse transform functions: 
 *                           plain C++ version
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

template<int skip, class T>void LeGall_5_3_invtransform_V_inplace(void *_idata,
                                                         const int istride,
                                                         const int width,
                                                         const int height) {
  T *idata = (T *)_idata;
  if (height < 4*skip) {
    for (int x = 0; x < width; x+=skip) {
      int32_t Dm1, D, Dp1;
      int y = 0;
      int32_t X, Xp1;
      D   = idata[y*istride + x];
      Dp1 = idata[(y + 1*skip)*istride + x];

      Dm1 = Dp1;
      {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        Xp1 = Dp1 + ((X   + X   + 1) >> 1);
        idata[(y + 0*skip)*istride + x] = X;
        idata[(y + 1*skip)*istride + x] = Xp1;
      }
    }
  } else if (height < 6*skip) {
    for (int x = 0; x < width; x+=skip) {
      int32_t Dm1, D, Dp1, Dp2, Dp3;
      int y = 0;
      int32_t X, Xp1, Xp2, Xp3;
      D   = idata[(y + 0*skip)*istride + x];
      Dp1 = idata[(y + 1*skip)*istride + x];
      Dp2 = idata[(y + 2*skip)*istride + x];
      Dp3 = idata[(y + 3*skip)*istride + x];

      Dm1 = Dp1;
      {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        Xp2 = Dp2 - ((Dp1 + Dp3 + 2) >> 2);
        Xp1 = Dp1 + ((X   + Xp2 + 1) >> 1);
        Xp3 = Dp3 + ((Xp2 + Xp2 + 1) >> 1);
        idata[(y + 0*skip)*istride + x] = X;
        idata[(y + 1*skip)*istride + x] = Xp1;
        idata[(y + 2*skip)*istride + x] = Xp2;
        idata[(y + 3*skip)*istride + x] = Xp3;
      }
    }
  } else {
    for (int x = 0; x < width; x+=skip) {

      int32_t Dm1, D, Dp1, Dp2;
      int y = 0;
      int32_t Xm2, Xm1, X;
      D   = idata[y*istride + x];
      Dp1 = idata[(y + 1*skip)*istride + x];
      Dp2 = idata[(y + 2*skip)*istride + x];

      Dm1 = Dp1;
      {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        idata[y*istride + x] = X;

        Xm2 = X;
        Dm1 = Dp1;
        D   = Dp2;
        Dp1 = idata[(y + 3*skip)*istride + x];
        Dp2 = idata[(y + 4*skip)*istride + x];
      }
      y += 2*skip;

      for (; y < height - 4*skip; y += 2*skip) {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

        idata[(y - 1*skip)*istride + x] = Xm1;
        idata[(y + 0*skip)*istride + x] = X;

        Xm2 = X;
        Dm1 = Dp1;
        D   = Dp2;
        Dp1 = idata[(y + 3*skip)*istride + x];
        Dp2 = idata[(y + 4*skip)*istride + x];
      }

      {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

        idata[(y - 1*skip)*istride + x] = Xm1;
        idata[(y + 0*skip)*istride + x] = X;

        Xm2 = X;
        Dm1 = Dp1;
        D   = Dp2;
        Dp1 = idata[(y + 3*skip)*istride + x];
      }
      y += 2*skip;

      {
        X   = D   - ((Dm1 + Dp1 + 2) >> 2);
        Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

        idata[(y - 1*skip)*istride + x] = Xm1;
        idata[(y + 0*skip)*istride + x] = X;

        Xm2 = X;
        Dm1 = Dp1;
        D   = D;
        Dp1 = Dp1;
      }
      y += 2*skip;

      {
        Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

        idata[(y - 1*skip)*istride + x] = Xm1;
      }
    }
  }
}

template<int skip, class T>void LeGall_5_3_invtransform_H_inplace(void *_idata,
                                                         const int istride,
                                                         const int width,
                                                         const int height) {
  T *idata = (T *)_idata;
  for (int y = 0; y < height; y+=skip) {

    int32_t Dm1, D, Dp1, Dp2;
    int x = 0;
    int32_t Xm2, Xm1, X;
    D   = idata[y*istride + x];
    Dp1 = idata[y*istride + x + 1*skip];
    Dp2 = idata[y*istride + x + 2*skip];

    Dm1 = Dp1;
    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      idata[y*istride + x] = (X + 1) >> 1;

      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
      Dp2 = idata[y*istride + x + 4*skip];
    }
    x += 2*skip;

    for (; x < width - 4*skip; x += 2*skip) {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      idata[y*istride + x - 1*skip] = (Xm1 + 1) >> 1;
      idata[y*istride + x + 0*skip] = (X   + 1) >> 1;

      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
      Dp2 = idata[y*istride + x + 4*skip];
    }

    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      idata[y*istride + x - 1*skip] = (Xm1 + 1) >> 1;
      idata[y*istride + x + 0*skip] = (X   + 1) >> 1;

      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      idata[y*istride + x - 1*skip] = (Xm1 + 1) >> 1;
      idata[y*istride + x + 0*skip] = (X   + 1) >> 1;

      Xm2 = X;
      Dm1 = Dp1;
      D   = D;
      Dp1 = Dp1;
    }
    x += 2*skip;

    {
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      idata[y*istride + x - 1*skip] = (Xm1 + 1) >> 1;
    }
  }
}

template<int active_bits, class T> void LeGall_5_3_invtransform_H_final_1(void *_idata,
                                                            const int istride,
                                                            const char *odata,
                                                            const int ostride,
                                                            const int iwidth,
                                                            const int iheight,
                                                            const int ooffset_x,
                                                            const int ooffset_y,
                                                            const int owidth,
                                                            const int oheight) {
  T *idata = (T *)_idata;
  const int skip = 1;
  const uint16_t clip = (1 << active_bits) - 1;
  const uint16_t offset = 1 << (active_bits - 1);

  for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y+=skip) {
    int32_t Dm1, D, Dp1, Dp2;
    int x = 0;
    int32_t Xm2, Xm1, X;
    D   = idata[y*istride + x];
    Dp1 = idata[y*istride + x + 1*skip];
    Dp2 = idata[y*istride + x + 2*skip];

    Dm1 = Dp1;
    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      if (x >= ooffset_x)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0*skip)] = (uint16_t)MIN(MAX((((X + 1) >> 1) + offset), 0), clip);

      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
      Dp2 = idata[y*istride + x + 4*skip];
    }
    x += 2*skip;

    for (; x < iwidth - 4*skip; x += 2*skip) {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      if (x - 1*skip >= ooffset_x && x - 1*skip < ooffset_x + owidth)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x - 1*skip)] = (uint16_t)MIN(MAX((((Xm1 + 1) >> 1) + offset), 0), clip);
      if (x  >= ooffset_x && x < ooffset_x + owidth)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0*skip)] = (uint16_t)MIN(MAX((((X   + 1) >> 1) + offset), 0), clip);


      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
      Dp2 = idata[y*istride + x + 4*skip];
    }

    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      if (x < ooffset_x + owidth) {
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x - 1*skip)] = (uint16_t)MIN(MAX((((Xm1 + 1) >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0*skip)] = (uint16_t)MIN(MAX((((X   + 1) >> 1) + offset), 0), clip);
      }

      Xm2 = X;
      Dm1 = Dp1;
      D   = Dp2;
      Dp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    {
      X   = D   - ((Dm1 + Dp1 + 2) >> 2);
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      if (x - 1 < ooffset_x + owidth)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x - 1*skip)] = (uint16_t)MIN(MAX((((Xm1 + 1) >> 1) + offset), 0), clip);
      if (x < ooffset_x + owidth)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0*skip)] = (uint16_t)MIN(MAX((((X   + 1) >> 1) + offset), 0), clip);

      Xm2 = X;
      Dm1 = Dp1;
      D   = D;
      Dp1 = Dp1;
    }
    x += 2*skip;

    {
      Xm1 = Dm1 + ((Xm2 + X   + 1) >> 1);

      if (x - 1 < ooffset_x + owidth)
        ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x - 1*skip)] = (uint16_t)MIN(MAX((((Xm1 + 1) >> 1) + offset), 0), clip);
    }
  }
}
