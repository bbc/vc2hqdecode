/*****************************************************************************
 * deslauriers_dubuc_9_7_invtransform.hpp : Deslaurier-Dubuc inverse 
 *                                          transform functions: plain C++ 
 *                                          version
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

template<int skip, class T>void Deslauriers_Dubuc_9_7_invtransform_V_inplace(void *_idata,
                                                                    const int istride,
                                                                    const int width,
                                                                    const int height) {
  T *idata = (T *)_idata;
  for (int x = 0; x < width; x += skip) {
    int y = 0;
    int32_t Xm3, Xm1, X, Xp1;
    int32_t Dm6, Dm4, Dm3, Dm2, D;

    X   = idata[(y + 0*skip)*istride + x];
    Xp1 = idata[(y + 1*skip)*istride + x];

    {
      Xm1 = Xp1;

      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm4 = D;
      Dm2 = D;

      Xm1 = Xp1;
      X   = idata[(y + 2*skip)*istride + x];
      Xp1 = idata[(y + 3*skip)*istride + x];
    }
    y += 2*skip;

    {
      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[(y + 2*skip)*istride + x];
      Xp1 = idata[(y + 3*skip)*istride + x];
    }
    y += 2*skip;

    for (; y < height - 2*skip; y += 2*skip) {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[(y - 4*skip)*istride + x] = Dm4;
      idata[(y - 3*skip)*istride + x] = Dm3;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[(y + 2*skip)*istride + x];
      Xp1 = idata[(y + 3*skip)*istride + x];
    }

    {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[(y - 4*skip)*istride + x] = Dm4;
      idata[(y - 3*skip)*istride + x] = Dm3;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    y += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[(y - 4*skip)*istride + x] = Dm4;
      idata[(y - 3*skip)*istride + x] = Dm3;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;
      D   = Dm6;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    y += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[(y - 4*skip)*istride + x] = Dm4;
      idata[(y - 3*skip)*istride + x] = Dm3;
    }
  }
}

template<int skip, class T>void Deslauriers_Dubuc_9_7_invtransform_H_inplace(void *_idata,
                                                                    const int istride,
                                                                    const int width,
                                                                    const int height) {
  T *idata = (T *)_idata;
  for (int y = 0; y < height; y += skip) {
    int x = 0;
    int32_t Xm3, Xm1, X, Xp1;
    int32_t Dm6, Dm4, Dm3, Dm2, D;

    X   = idata[y*istride + x + 0*skip];
    Xp1 = idata[y*istride + x + 1*skip];

    {
      Xm1 = Xp1;

      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm4 = D;
      Dm2 = D;

      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    {
      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    for (; x < width - 2*skip; x += 2*skip) {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[y*istride + x - 4*skip] = Dm4 >> 1;
      idata[y*istride + x - 3*skip] = Dm3 >> 1;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }

    {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[y*istride + x - 4*skip] = Dm4 >> 1;
      idata[y*istride + x - 3*skip] = Dm3 >> 1;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    x += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[y*istride + x - 4*skip] = Dm4 >> 1;
      idata[y*istride + x - 3*skip] = Dm3 >> 1;

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;
      D   = Dm6;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    x += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      idata[y*istride + x - 4*skip] = Dm4 >> 1;
      idata[y*istride + x - 3*skip] = Dm3 >> 1;
    }
  }
}

template<int active_bits, class T> void Deslauriers_Dubuc_9_7_invtransform_H_final_1(void *_idata,
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
    int x = 0;
    int32_t Xm3, Xm1, X, Xp1;
    int32_t Dm6, Dm4, Dm3, Dm2, D;

    X   = idata[y*istride + x + 0*skip];
    Xp1 = idata[y*istride + x + 1*skip];

    {
      Xm1 = Xp1;

      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm4 = D;
      Dm2 = D;

      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    {
      D = X - ((Xm1 + Xp1 + 2) >> 2);

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }
    x += 2*skip;

    for (; x < iwidth - 2*skip; x += 2*skip) {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      if (x >= ooffset_x + 4*skip && x < ooffset_x + owidth + 3*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 4*skip] = (uint16_t)MIN(MAX(((Dm4 >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Dm3 >> 1) + offset), 0), clip);
      }

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
      X   = idata[y*istride + x + 2*skip];
      Xp1 = idata[y*istride + x + 3*skip];
    }

    {
      D   = X   - ((Xm1 + Xp1 + 2) >> 2);
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      if (x >= ooffset_x + 4*skip && x < ooffset_x + owidth + 3*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 4*skip] = (uint16_t)MIN(MAX(((Dm4 >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Dm3 >> 1) + offset), 0), clip);
      }

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    x += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      if (x >= ooffset_x + 4*skip && x < ooffset_x + owidth + 3*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 4*skip] = (uint16_t)MIN(MAX(((Dm4 >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Dm3 >> 1) + offset), 0), clip);
      }

      Dm6 = Dm4;
      Dm4 = Dm2;
      Dm2 = D;
      D   = Dm6;

      Xm3 = Xm1;
      Xm1 = Xp1;
    }
    x += 2*skip;

    {
      Dm3 = Xm3 + ((-Dm6 + 9*Dm4 + 9*Dm2 - D + 8) >> 4);

      if (x >= ooffset_x + 4*skip && x < ooffset_x + owidth + 3*skip) {
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 4*skip] = (uint16_t)MIN(MAX(((Dm4 >> 1) + offset), 0), clip);
        ((uint16_t *)odata)[(y - ooffset_y)*ostride + (x - ooffset_x) - 3*skip] = (uint16_t)MIN(MAX(((Dm3 >> 1) + offset), 0), clip);
      }
    }
  }
}
