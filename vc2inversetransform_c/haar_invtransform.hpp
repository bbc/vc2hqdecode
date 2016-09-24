/*****************************************************************************
 * haar_invtransform.hpp : Haar filter inverse transform functions: 
 *                         plain C++ version
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



template<int skip, class T>void Haar_invtransform_V_inplace(void *_idata,
                                                            const int istride,
                                                            const int width,
                                                            const int height) {
  T *idata = (T *)_idata;
  for (int y = 0; y < height; y += 2*skip) {
    for (int x = 0; x < width; x+=skip) {
      int32_t D   = idata[(y + 0*skip)*istride + x];
      int32_t Dp1 = idata[(y + 1*skip)*istride + x];

      int32_t X   = D   - (( Dp1 + 1 ) >> 1 );
      int32_t Xp1 = Dp1 + X;

      idata[(y + 0*skip)*istride + x] = X;
      idata[(y + 1*skip)*istride + x] = Xp1;
    }
  }
}

template<int skip, int shift, class T>void Haar_invtransform_H_inplace(void *_idata,
                                                                       const int istride,
                                                                       const int width,
                                                                       const int height) {
  T *idata = (T *)_idata;
  for (int y = 0; y < height; y+=skip) {
    for (int x = 0; x < width; x += 2*skip) {
      int32_t D   = idata[y*istride + x + 0*skip];
      int32_t Dp1 = idata[y*istride + x + 1*skip];

      int32_t X   = D   - (( Dp1 + 1 ) >> 1 );
      int32_t Xp1 = Dp1 + X;

      if (shift != 0) {
        X   +=  (1 << (shift - 1));
        X   >>= shift;
        Xp1 +=  (1 << (shift - 1));
        Xp1 >>= shift;
      }

      idata[y*istride + x + 0*skip] = X;
      idata[y*istride + x + 1*skip] = Xp1;
    }
  }
}

template<int shift, class T> void Haar_invtransform_H_final_1_10(void *_idata,
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
  for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y+=skip) {
    for (int x = ooffset_x; x < iwidth && x < ooffset_x + owidth; x += 2*skip) {
      int32_t D   = idata[y*istride + x + 0*skip];
      int32_t Dp1 = idata[y*istride + x + 1*skip];

      int32_t X   = D   - (( Dp1 + 1 ) >> 1 );
      int32_t Xp1 = Dp1 + X;

      if (shift != 0) {
        X   +=  (1 << (shift - 1));
        X   >>= shift;
        Xp1 +=  (1 << (shift - 1));
        Xp1 >>= shift;
      }

      ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0*skip)] = (uint16_t)MIN(MAX(((X) + (1 << 9)), 0), 0x3ff);
      ((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 1*skip)] = (uint16_t)MIN(MAX(((Xp1) + (1 << 9)), 0), 0x3ff);
    }
  }
}

template<int shift, class T> void Haar_invtransform_H_final_1_12(void *_idata,
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
	for (int y = ooffset_y; y < iheight && y < ooffset_y + oheight; y += skip) {
		for (int x = ooffset_x; x < iwidth && x < ooffset_x + owidth; x += 2 * skip) {
			int32_t D = idata[y*istride + x + 0 * skip];
			int32_t Dp1 = idata[y*istride + x + 1 * skip];

			int32_t X = D - ((Dp1 + 1) >> 1);
			int32_t Xp1 = Dp1 + X;

			if (shift != 0) {
				X += (1 << (shift - 1));
				X >>= shift;
				Xp1 += (1 << (shift - 1));
				Xp1 >>= shift;
			}

			((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 0 * skip)] = (uint16_t)MIN(MAX(((X)+(1 << 11)), 0), 0xfff);
			((uint16_t *)odata)[((y - ooffset_y)*ostride + x - ooffset_x + 1 * skip)] = (uint16_t)MIN(MAX(((Xp1)+(1 << 11)), 0), 0xfff);
		}
	}
}
