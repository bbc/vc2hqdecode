/*****************************************************************************
 * invtransform_c.cpp : Inverse transform functions: plain C++ version
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

#include "invtransform_c.hpp"
#include "logger.hpp"
#include "legall_invtransform.hpp"
#include "haar_invtransform.hpp"
#include "deslauriers_dubuc_9_7_invtransform.hpp"
#include "deslauriers_dubuc_13_7_invtransform.hpp"
#include "fidelity_invtransform.hpp"

InplaceTransform get_invhtransform_c(int wavelet_index, int level, int depth, int sample_size) {
  if (sample_size == 4) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_FIDELITY:
      switch (depth - level - 1) {
      case 3:
        return Fidelity_invtransform_H_inplace<8>;
      case 2:
        return Fidelity_invtransform_H_inplace<4>;
      case 1:
        return Fidelity_invtransform_H_inplace<2>;
      case 0:
        return Fidelity_invtransform_H_inplace<1>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<8, int32_t>;
      case 2:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<4, int32_t>;
      case 1:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<2, int32_t>;
      case 0:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<8, int32_t>;
      case 2:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<4, int32_t>;
      case 1:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<2, int32_t>;
      case 0:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_LEGALL_5_3:
      switch(depth - level - 1) {
      case 3:
        return LeGall_5_3_invtransform_H_inplace<8, int32_t>;
      case 2:
        return LeGall_5_3_invtransform_H_inplace<4, int32_t>;
      case 1:
        return LeGall_5_3_invtransform_H_inplace<2, int32_t>;
      case 0:
        return LeGall_5_3_invtransform_H_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
      switch (depth - level - 1) {
      case 3:
        return Haar_invtransform_H_inplace<8,0, int32_t>;
      case 2:
        return Haar_invtransform_H_inplace<4,0, int32_t>;
      case 1:
        return Haar_invtransform_H_inplace<2,0, int32_t>;
      case 0:
        return Haar_invtransform_H_inplace<1,0, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch (depth - level - 1) {
      case 3:
        return Haar_invtransform_H_inplace<8,1, int32_t>;
      case 2:
        return Haar_invtransform_H_inplace<4,1, int32_t>;
      case 1:
        return Haar_invtransform_H_inplace<2,1, int32_t>;
      case 0:
        return Haar_invtransform_H_inplace<1,1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  } else if (sample_size == 2) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<8, int16_t>;
      case 2:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<4, int16_t>;
      case 1:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<2, int16_t>;
      case 0:
        return Deslauriers_Dubuc_9_7_invtransform_H_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<8, int16_t>;
      case 2:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<4, int16_t>;
      case 1:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<2, int16_t>;
      case 0:
        return Deslauriers_Dubuc_13_7_invtransform_H_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_LEGALL_5_3:
      switch (depth - level - 1) {
      case 3:
        return LeGall_5_3_invtransform_H_inplace<8, int16_t>;
      case 2:
        return LeGall_5_3_invtransform_H_inplace<4, int16_t>;
      case 1:
        return LeGall_5_3_invtransform_H_inplace<2, int16_t>;
      case 0:
        return LeGall_5_3_invtransform_H_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
      switch (depth - level - 1) {
      case 3:
        return Haar_invtransform_H_inplace<8,0, int16_t>;
      case 2:
        return Haar_invtransform_H_inplace<4,0, int16_t>;
      case 1:
        return Haar_invtransform_H_inplace<2,0, int16_t>;
      case 0:
        return Haar_invtransform_H_inplace<1,0, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch (depth - level - 1) {
      case 3:
        return Haar_invtransform_H_inplace<8,1, int16_t>;
      case 2:
        return Haar_invtransform_H_inplace<4,1, int16_t>;
      case 1:
        return Haar_invtransform_H_inplace<2,1, int16_t>;
      case 0:
        return Haar_invtransform_H_inplace<1,1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  }

  writelog(LOG_ERROR, "%s:%d:  Invalid sample size\n", __FILE__, __LINE__);
  throw VC2DECODER_NOTIMPLEMENTED;
}

InplaceTransform get_invvtransform_c(int wavelet_index, int level, int depth, int sample_size) {
  if (sample_size == 4) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_FIDELITY:
      switch (depth - level - 1) {
      case 3:
        return Fidelity_invtransform_V_inplace<8>;
      case 2:
        return Fidelity_invtransform_V_inplace<4>;
      case 1:
        return Fidelity_invtransform_V_inplace<2>;
      case 0:
        return Fidelity_invtransform_V_inplace<1>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<8, int32_t>;
      case 2:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<4, int32_t>;
      case 1:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<2, int32_t>;
      case 0:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<8, int32_t>;
      case 2:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<4, int32_t>;
      case 1:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<2, int32_t>;
      case 0:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_LEGALL_5_3:
      switch (depth - level - 1) {
      case 3:
        return LeGall_5_3_invtransform_V_inplace<8, int32_t>;
      case 2:
        return LeGall_5_3_invtransform_V_inplace<4, int32_t>;
      case 1:
        return LeGall_5_3_invtransform_V_inplace<2, int32_t>;
      case 0:
        return LeGall_5_3_invtransform_V_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch(depth - level - 1) {
      case 3:
        return Haar_invtransform_V_inplace<8, int32_t>;
      case 2:
        return Haar_invtransform_V_inplace<4, int32_t>;
      case 1:
        return Haar_invtransform_V_inplace<2, int32_t>;
      case 0:
        return Haar_invtransform_V_inplace<1, int32_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  } else if (sample_size == 2) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<8, int16_t>;
      case 2:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<4, int16_t>;
      case 1:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<2, int16_t>;
      case 0:
        return Deslauriers_Dubuc_13_7_invtransform_V_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
      break;
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (depth - level - 1) {
      case 3:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<8, int16_t>;
      case 2:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<4, int16_t>;
      case 1:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<2, int16_t>;
      case 0:
        return Deslauriers_Dubuc_9_7_invtransform_V_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
      break;
    case VC2DECODER_WFT_LEGALL_5_3:
      switch (depth - level - 1) {
      case 3:
        return LeGall_5_3_invtransform_V_inplace<8, int16_t>;
      case 2:
        return LeGall_5_3_invtransform_V_inplace<4, int16_t>;
      case 1:
        return LeGall_5_3_invtransform_V_inplace<2, int16_t>;
      case 0:
        return LeGall_5_3_invtransform_V_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch(depth - level - 1) {
      case 3:
        return Haar_invtransform_V_inplace<8, int16_t>;
      case 2:
        return Haar_invtransform_V_inplace<4, int16_t>;
      case 1:
        return Haar_invtransform_V_inplace<2, int16_t>;
      case 0:
        return Haar_invtransform_V_inplace<1, int16_t>;
      default:
        writelog(LOG_ERROR, "%s:%d:  Invalid transform depth\n", __FILE__, __LINE__);
        throw VC2DECODER_NOTIMPLEMENTED;
      }
      break;
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  }

  writelog(LOG_ERROR, "%s:%d:  Invalid sample size\n", __FILE__, __LINE__);
  throw VC2DECODER_NOTIMPLEMENTED;
}

InplaceTransformFinal get_invhtransformfinal_c(int wavelet_index, int active_bits, int sample_size) {
  if (active_bits < 10 || active_bits > 16 || (active_bits % 2) != 0) {
    writelog(LOG_ERROR, "%s:%d:  Invalid bit depth\n", __FILE__, __LINE__);
    throw VC2DECODER_NOTIMPLEMENTED;
  }

  if (sample_size == 4) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_FIDELITY:
      switch (active_bits) {
      case 10: return Fidelity_invtransform_H_final_1<10>;
      case 12: return Fidelity_invtransform_H_final_1<12>;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (active_bits) {
      case 10: return Deslauriers_Dubuc_9_7_invtransform_H_final_1<10, int32_t>;
      case 12: return Deslauriers_Dubuc_9_7_invtransform_H_final_1<12, int32_t>;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (active_bits) {
      case 10: return Deslauriers_Dubuc_13_7_invtransform_H_final_1<10, int32_t>;
      case 12: return Deslauriers_Dubuc_13_7_invtransform_H_final_1<12, int32_t>;
      }
    case VC2DECODER_WFT_LEGALL_5_3:
      switch (active_bits) {
      case 10: return LeGall_5_3_invtransform_H_final_1<10, int32_t>;
      case 12: return LeGall_5_3_invtransform_H_final_1<12, int32_t>;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
      switch (active_bits) {
        case 10: return Haar_invtransform_H_final_1<0, 10, int32_t>;
        case 12: return Haar_invtransform_H_final_1<0, 12, int32_t>;
      }
      break;
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch (active_bits) {
        case 10: return Haar_invtransform_H_final_1<1, 10, int32_t>;
        case 12: return Haar_invtransform_H_final_1<1, 12, int32_t>;
      }
      break;
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  } else if (sample_size == 2) {
    switch(wavelet_index) {
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7:
      switch (active_bits) {
      case 10: return Deslauriers_Dubuc_9_7_invtransform_H_final_1<10, int16_t>;
      case 12: return Deslauriers_Dubuc_9_7_invtransform_H_final_1<12, int16_t>;
      }
    case VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7:
      switch (active_bits) {
      case 10: return Deslauriers_Dubuc_13_7_invtransform_H_final_1<10, int16_t>;
      case 12: return Deslauriers_Dubuc_13_7_invtransform_H_final_1<12, int16_t>;
      }
    case VC2DECODER_WFT_LEGALL_5_3:
      switch (active_bits) {
      case 10: return LeGall_5_3_invtransform_H_final_1<10, int16_t>;
      case 12: return LeGall_5_3_invtransform_H_final_1<12, int16_t>;
      }
    case VC2DECODER_WFT_HAAR_NO_SHIFT:
      switch (active_bits) {
        case 10: return Haar_invtransform_H_final_1<0, 10, int16_t>;
        case 12: return Haar_invtransform_H_final_1<0, 12, int16_t>;
      }
      break;
    case VC2DECODER_WFT_HAAR_SINGLE_SHIFT:
      switch (active_bits) {
        case 10: return Haar_invtransform_H_final_1<1, 10, int16_t>;
        case 12: return Haar_invtransform_H_final_1<1, 12, int16_t>;
      }
      break;
    default:
      writelog(LOG_ERROR, "%s:%d:  Invalid transform\n", __FILE__, __LINE__);
      throw VC2DECODER_NOTIMPLEMENTED;
    }
  }

  writelog(LOG_ERROR, "%s:%d:  Invalid sample size\n", __FILE__, __LINE__);
  throw VC2DECODER_NOTIMPLEMENTED;
}
