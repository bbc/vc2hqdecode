/*****************************************************************************
 * internal.h : internal data structures and defines needed by the decoder
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

#ifndef __INTERNAL_HPP__
#define __INTERNAL_HPP__

#include <stdint.h>
#define VC2HQDECODE_DLL
#include "vc2hqdecode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DWT_DEPTH 8

typedef struct _VC2DecoderParamsInternal {
  VC2DecoderVideoFormat     video_format;
  VC2DecoderTransformParams transform_params;

  int slice_prefix_bytes;
  int slice_size_scalar;
  int threads;
  int numa_first_node;

  bool colourise;
  bool colourise_quantiser;
  bool colourise_padding;
  bool colourise_unpadded;

  bool partial_decode;
  int partial_decode_offset_x;
  int partial_decode_offset_y;
  int partial_decode_width;
  int partial_decode_height;
} VC2DecoderParamsInternal;

enum _VC2DecoderEndianness {
  VC2DECODER_ENDIANNESS_LE = 0,
  VC2DECODER_ENDIANNESS_BE = 1
};

enum _VC2DecoderComponent {
  VC2DECODER_COMP_Y  = 0,
  VC2DECODER_COMP_Cb = 1,
  VC2DECODER_COMP_Cr = 2
};

typedef enum _VC2DecoderSequenceResult {
  VC2DECODER_EOS             = 1,
  VC2DECODER_RECONFIGURED    = 2,
  VC2DECODER_PICTURE         = 3,
  VC2DECODER_AUXILIARY       = 4,
  VC2DECODER_INVALID_PICTURE = 5,
} VC2DecoderSequenceResult;

/* Deprecated interface no longer recommended */
const uint8_t VC2DECODER_PARSE_CODE_SEQUENCE_HEADER  = 0x00;
const uint8_t VC2DECODER_PARSE_CODE_END_OF_SEQUENCE  = 0x10;
const uint8_t VC2DECODER_PARSE_CODE_AUXILIARY_DATA   = 0x20;
const uint8_t VC2DECODER_PARSE_CODE_PADDING_DATA     = 0x30;
const uint8_t VC2DECODER_PARSE_CODE_CORE_PICTURE_AC  = 0x08;
const uint8_t VC2DECODER_PARSE_CODE_CORE_PICTURE_VLC = 0x48;
const uint8_t VC2DECODER_PARSE_CODE_LD_PICTURE       = 0xC8;
const uint8_t VC2DECODER_PARSE_CODE_HQ_PICTURE       = 0xE8;
const uint8_t VC2DECODER_PARSE_CODE_LD_FRAGMENT      = 0xCC;
const uint8_t VC2DECODER_PARSE_CODE_HQ_FRAGMENT      = 0xEC;

typedef struct _VC2DecoderParseSegment {
  uint8_t parse_code;
  char    *data;
  char    *next_header;
  char    *prev_header;
  int     data_length;
} VC2DecoderParseSegment;

#ifdef __cplusplus
};
#endif


#endif /* __INTERNAL_HPP__ */
