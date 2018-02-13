/*****************************************************************************
 * VC2Decoder.hpp : Main class header file for decoder
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

#ifndef __VC2DECODER_HPP__
#define __VC2DECODER_HPP__

#include <stdlib.h>

#include "internal.h"
#include "VideoFormat.hpp"
#include "vlc.hpp"
#include "dequantise.hpp"
#include "datastructures.hpp"
#include "invtransform.hpp"
#include "ThreadPool.hpp"

void detect_cpu_features();

class VC2Decoder {
public:
  VC2Decoder() {
    mSlicesX = 0;
    mSlicesY = 0;
    mMatrices = NULL;

    mJobsX = 0;
    mJobsY = 0;
    mJobs = NULL;
    mPool = NULL;
    mThreads = 0;

    mTransformParamsEncoded = NULL;
    mTransformParamsEncodedLength = 0;
    mSeqHeaderEncoded = NULL;
    mSeqHeaderEncodedLength = 0;

    mConfigured = false;

    transforms_h = NULL;
    transforms_v = NULL;
    mDequant[0] = NULL;
    mDequant[1] = NULL;
    mDequant[2] = NULL;

    mSliceDecoder = NULL;

    mSliceJobLUTX = NULL;
    mSliceJobLUTY = NULL;

    memset(&mSequenceInfo, 0, sizeof(VC2DecoderSequenceInfo));
    memset(&mParams, 0, sizeof(mParams));
    memset(&mVideoFormat, 0, sizeof(mVideoFormat));
    memset(&mOutputFormat, 0, sizeof(mOutputFormat));

    mSampleSize = 0;
    mMajorVersion = 0;
  }

  ~VC2Decoder() {
    if (mJobs) {
      for (int i = 0; i < mJobsX*mJobsY; i++)
        if (mJobs[i])
          delete mJobs[i];
      delete[] mJobs;
    }
    if (mMatrices)
      delete_matrices(mMatrices);
    if (mPool) {
      mPool->stop();
      delete mPool;
    }
    if (mTransformParamsEncoded) {
      delete[] mTransformParamsEncoded;
    }
    if (mSeqHeaderEncoded) {
      delete[] mSeqHeaderEncoded;
    }

    if (mSliceJobLUTX) {
      delete[] mSliceJobLUTX;
    }

    if (mSliceJobLUTY) {
      delete[] mSliceJobLUTY;
    }

    if (transforms_v)
      delete[] transforms_v;
    if (transforms_h)
      delete[] transforms_h;
  }

  VC2DecoderSequenceInfo getSequenceInfo() { return mSequenceInfo; }

  void setUserParams(VC2DecoderParamsUser &params);
  void setVideoFormat(VC2DecoderParamsInternal &params);
  void setParams(VC2DecoderParamsInternal &params);
  bool parseSeqHeader(char *_idata, const char *end);
  VC2DecoderOutputFormat getOutputFormat() { return mOutputFormat; }

  uint64_t decodeFrame(char *idata, int ilength, uint16_t **odata, int *ostride);
  bool handleFragment(char *idata, int ilength, uint16_t **odata, int *ostride);

  char *FindNextParseInfo(char *_idata, int ilength);
  /*
     processes stream starting at idata and not progressing longer than ilength,
     seaks through stream, finds Sequence Header, processes. Advances idata.
   */
  VC2DecoderSequenceResult sequenceSynchronise(char **idata, int ilength, bool skip_aux);
  VC2DecoderSequenceResult sequenceDecodeOnePicture(char **idata, int ilength, uint16_t **odata, int *ostride, bool skip_aux);
  int sequenceExtractAux(char **idata, int ilength, uint8_t **odata);

protected:
  int processTransformParams(uint8_t *_idata, int ilength) throw (VC2DecoderResult);

  uint64_t SliceInput(char *idata, int ilength, JobData **jobs);
  uint64_t SliceInputFragment(char *idata, int ilength, int n_slices, int x_offset, int y_offset, JobData **jobs);

  void Decode(JobData *, uint16_t **odata, int *ostride);

  VC2DecoderParamsInternal mParams;
  vc2::VideoFormat mVideoFormat;

  int mSlicesX;
  int mSlicesY;

  JobData **mJobs;
  int mJobsX;
  int mJobsY;

  int mOverlapX;

  QuantisationMatrix *mMatrices;

  ThreadPool *mPool;
  int mThreads;

  uint8_t *mTransformParamsEncoded;
  int mTransformParamsEncodedLength;

  uint8_t *mSeqHeaderEncoded;
  int mSeqHeaderEncodedLength;

  bool mConfigured;

  VC2DecoderOutputFormat mOutputFormat;

  InplaceTransform *transforms_h;
  InplaceTransform *transforms_v;
  InplaceTransformFinal transforms_final;

  DequantiseFunction mDequant[3];
  SliceDecoderFunc mSliceDecoder;

  uint8_t *mSliceJobLUTX;
  uint8_t *mSliceJobLUTY;

  bool mInterlaced;

  int mWidth;
  int mHeight;

  VC2DecoderSequenceInfo mSequenceInfo;

  int mSampleSize;

  int mSlicesSlicedFromFragments;
  int mMajorVersion;
};

#endif /* __VC2DECODER_HPP__ */
