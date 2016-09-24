/*****************************************************************************
 * invtransform_sse4_2.cpp : Inverse transform functions: SSE4.2 version
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

#include "../vc2inversetransform_c/invtransform_c.hpp"
#include "invtransform_sse4_2.hpp"
#include "logger.hpp"
#include "legall_invtransform.hpp"
#include "haar_invtransform.hpp"

InplaceTransform get_invhtransform_sse4_2(int wavelet_index, int level, int depth, int sample_size) {
  if (sample_size == 4) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_LEGALL_5_3:
      if (depth - level - 1 == 1)
        return LeGall_5_3_invtransform_H_inplace_2_sse4_2;
      else if (depth - level - 1 == 0)
        return LeGall_5_3_invtransform_H_inplace_1_sse4_2;
      break;
    default:
      break;
    }
  }

  return get_invhtransform_c(wavelet_index, level, depth, sample_size);
}

InplaceTransform get_invvtransform_sse4_2(int wavelet_index, int level, int depth, int sample_size) {
  if (sample_size == 4) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_LEGALL_5_3:
      if (depth - level - 1 == 1)
        return LeGall_5_3_invtransform_V_inplace_sse4_2_int32_t<2>;
      else if (depth - level - 1 == 0)
        return LeGall_5_3_invtransform_V_inplace_sse4_2_int32_t<1>;
      break;
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      if (depth - level - 1 == 0) {
        return Haar_invtransform_V_inplace_sse4_2<1>;
      }
      break;
    default:
      break;
    }
  } else if (sample_size == 2) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_LEGALL_5_3:
      if (depth - level - 1 == 1)
        return LeGall_5_3_invtransform_V_inplace_sse4_2_int16_t<2>;
      else if (depth - level - 1 == 0)
        return LeGall_5_3_invtransform_V_inplace_sse4_2_int16_t<1>;
      break;
    default:
      break;
    }
  }

  return get_invvtransform_c(wavelet_index, level, depth, sample_size);
}

InplaceTransformFinal get_invhtransformfinal_sse4_2(int wavelet_index, int active_bits, int sample_size) {
  if (sample_size == 4) {
    if (active_bits == 10) {
      switch(wavelet_index) {
      case VC2DECODER_WFT_LEGALL_5_3:
        return LeGall_5_3_invtransform_H_final_1_10_sse4_2<int32_t>;
      case VC2DECODER_WFT_HAAR_NO_SHIFT:
        return Haar_invtransform_H_final_1_10_sse4_2_int32_t<0>;
      case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
        return Haar_invtransform_H_final_1_10_sse4_2_int32_t<1>;
      default:
        break;
      }
	}
	else if (active_bits == 12) {
		switch (wavelet_index) {
		case VC2DECODER_WFT_HAAR_NO_SHIFT:
			return Haar_invtransform_H_final_1_12_sse4_2_int32_t<0>;
		case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
			return Haar_invtransform_H_final_1_12_sse4_2_int32_t<1>;
		default:
			break;
		}
	}
  } else if (sample_size == 2) {
    if (active_bits == 10) {
      switch(wavelet_index) {
      case VC2DECODER_WFT_LEGALL_5_3:
        return LeGall_5_3_invtransform_H_final_1_10_sse4_2<int16_t>;
      case VC2DECODER_WFT_HAAR_NO_SHIFT:
        return Haar_invtransform_H_final_1_10_sse4_2_int16_t<0>;
      case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
        return Haar_invtransform_H_final_1_10_sse4_2_int16_t<1>;
      default:
        break;
      }
	}
	else if (active_bits == 12) {
		switch (wavelet_index) {
		case VC2DECODER_WFT_HAAR_NO_SHIFT:
			return Haar_invtransform_H_final_1_12_sse4_2_int16_t<0>;
		case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
			return Haar_invtransform_H_final_1_12_sse4_2_int16_t<1>;
		default:
			break;
		}
	}
  }

  return get_invhtransformfinal_c(wavelet_index, active_bits, sample_size);
}
