/*****************************************************************************
 * datastructures.hpp : general data structures
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

#ifndef __DATA_STRUCTURES_HPP__
#define __DATA_STRUCTURES_HPP__

#include <cstdlib>

#include <cstdio>

#include <cstring>

#include "platform_variant.hpp"

struct CodedSlice {
public:
  CodedSlice() {
    data[0] = NULL;
    data[1] = NULL;
    data[2] = NULL;
    length[0] = 0;
    length[1] = 0;
    length[2] = 0;
  }


  char *data[3];
  int length[3];
  int qindex;
  int padding;
};

class DecodedSlice {
public:
  DecodedSlice(int S) {
    size = S;
    const int PAD = 8;
    const int alloc_size = ((S + PAD + 15)/16)*16;
    mAllocatedData = ALIGNED_ALLOC(16, sizeof(uint32_t)*alloc_size);
    data           = (int32_t *)mAllocatedData;
  }

  ~DecodedSlice() {
    ALIGNED_FREE(mAllocatedData);
  }

  int size;
  int32_t *data;

protected:
  void *mAllocatedData;
};

struct VideoPlane {
  VideoPlane(int w, int h, int sample_size) {
    width  = w;
    height = h;
    int align_size = 64/sample_size;
    stride = (((w + align_size - 1)/align_size)*align_size); // Integer number of cache lines
    int allocsize = 2*stride*height*sample_size;
    data = (void *)ALIGNED_ALLOC(16, allocsize);
  }

  ~VideoPlane() {
    ALIGNED_FREE(data);
  }

  template <class T> T *as() { return (T*)data; }

  void *data;
  int stride;
  int height;
  int width;
};

struct JobData {
  JobData (int n,
           int _width, int _height, int _fullheight,
           int _slices_x, int _slices_y,
           int off_x, int off_y,
           int outw,  int outh,
           int tgt_x, int tgt_y,
           int _slice_start_x, int _slice_start_y,
           int sample_size) {
    number = n;
    width[0] = _width;
    width[1] = _width/2;
    width[2] = _width/2;

    height[0] = _height;
    height[1] = _height;
    height[2] = _height;

    fullheight[0] = _fullheight;
    fullheight[1] = _fullheight;
    fullheight[2] = _fullheight;

    slices_x = _slices_x;
    slices_y = _slices_y;

    output_x[0] = off_x;
    output_x[1] = off_x/2;
    output_x[2] = off_x/2;

    output_y[0] = off_y;
    output_y[1] = off_y;
    output_y[2] = off_y;

    output_w[0] = outw;
    output_w[1] = outw/2;
    output_w[2] = outw/2;

    output_h[0] = outh;
    output_h[1] = outh;
    output_h[2] = outh;

    ostride[0] = 0;
    ostride[1] = 0;
    ostride[2] = 0;

    odata[0] = NULL;
    odata[1] = NULL;
    odata[2] = NULL;

    coded_slices  = new CodedSlice[slices_x*slices_y];
    decoded_slice[0] = new DecodedSlice(width[0]*height[0]/_slices_x/_slices_y);
    decoded_slice[1] = new DecodedSlice(width[1]*height[1]/_slices_x/_slices_y);
    decoded_slice[2] = new DecodedSlice(width[2]*height[2]/_slices_x/_slices_y);
    video_data[0] = new VideoPlane(width[0], height[0], sample_size);
    video_data[1] = new VideoPlane(width[1], height[1], sample_size);
    video_data[2] = new VideoPlane(width[2], height[2], sample_size);

    target_x[0] = tgt_x;
    target_x[1] = tgt_x/2;
    target_x[2] = tgt_x/2;
    target_y[0] = tgt_y;
    target_y[1] = tgt_y;
    target_y[2] = tgt_y;

    slice_start_x = _slice_start_x;
    slice_start_y = _slice_start_y;
  }

  ~JobData() {
    delete[] coded_slices;
    delete decoded_slice[0];
    delete decoded_slice[1];
    delete decoded_slice[2];
    delete video_data[0];
    delete video_data[1];
    delete video_data[2];
  }
  
  CodedSlice *coded_slices;
  DecodedSlice *decoded_slice[3];
  VideoPlane *video_data[3];

  int number;

  int slice_start_x;
  int slice_start_y;

  int slices_x;
  int slices_y;

  int width[3];
  int height[3];
  int fullheight[3];

  char *odata[3];
  int ostride[3];
  int output_x[3];
  int output_y[3];
  int output_w[3];
  int output_h[3];

  int target_x[3];
  int target_y[3];
};

#endif /* __DATA_STRUCTURES_HPP__ */
