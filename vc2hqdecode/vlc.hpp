/*****************************************************************************
 * vlc.hpp : VLC decoding
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

#ifndef __VLC_HPP__
#define __VLC_HPP__

#include <stdlib.h>
#include <stdint.h>

#include "lut.hpp"
#include "datastructures.hpp"
#include "dequantise.hpp"

typedef void (*SliceDecoderFunc)(QuantisationMatrix *matrices,
                                 CodedSlice * const input,
                                 DecodedSlice ** scratch,
                                 int n_slices_x,
                                 int n_slices_y,
                                 VideoPlane **video_data,
                                 int slice_width,
                                 int slice_height,
                                 int depth,
                                 DequantiseFunction *dequant);

typedef SliceDecoderFunc (*GetSliceDecoderFunc)(int sample_size);

/* These are noddy implementations used only in decoding configuration data */
inline bool     read_bool(uint8_t *&data, int &bitnum) {
  bool r;
  if ((((*data) >> bitnum)&0x1) == 0x1)
    r = true;
  else
    r = false;

  if (bitnum == 0) {
    bitnum = 7;
    data++;
  } else {
    bitnum--;
  }
  return r;
}

inline void byte_align(uint8_t *&data, int &bitnum) {
  if (bitnum != 7) {
    bitnum = 7;
    data++;
  }
}

inline uint32_t read_uint(uint8_t *&data, int &bitnum) {
  uint32_t r = 1;
  while(!read_bool(data, bitnum)) {
    r <<= 1;
    if (read_bool(data, bitnum)) {
      r |= 1;
    }
  }
  return (r-1);
}

#endif /* __VLC_HPP__ */
