/*****************************************************************************
 * vc2hqdecode.cpp : external interface functions
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

#include "internal.h"
#include "VC2Decoder.hpp"
#include "stream.hpp"

#define VC2DECODER_BEGIN \
  try {\
    VC2Decoder *decoder = (VC2Decoder *)handle;

#define VC2DECODER_END \
  } catch (VC2DecoderResult &r) { \
    return r;\
  } catch (...) { \
    return VC2DECODER_UNKNOWN_ERROR;\
  }

void vc2decode_init() {
  detect_cpu_features();
}

VC2DecoderHandle vc2decode_create() {
   VC2Decoder *decoder = new VC2Decoder();
  return (VC2DecoderHandle)decoder;
}

VC2DecoderResult vc2decode_parse_info(char *data, VC2DecoderParseSegment *seg) {
  try {

    *seg = parse_info(data);
    return VC2DECODER_OK;

  } catch (VC2DecoderResult &r) {
    return r;
  } catch (...) {
    return VC2DECODER_UNKNOWN_ERROR;
  }
}

VC2DecoderResult vc2decode_get_output_format(VC2DecoderHandle handle, VC2DecoderOutputFormat *fmt) {
  VC2DECODER_BEGIN

  *fmt = decoder->getOutputFormat();
  return VC2DECODER_OK;

  VC2DECODER_END
}

VC2DecoderResult vc2decode_set_parameters(VC2DecoderHandle handle, VC2DecoderParamsUser params) {
  VC2DECODER_BEGIN

  decoder->setUserParams(params);
  return VC2DECODER_OK;

  VC2DECODER_END
}

VC2DecoderResult vc2decode_parse_seqheader(VC2DecoderHandle handle, char *idata) {
  VC2DECODER_BEGIN

  decoder->parseSeqHeader(idata);
  return VC2DECODER_OK;

  VC2DECODER_END
}

VC2DecoderResult vc2decode_decode_frame(VC2DecoderHandle handle, char *idata, int ilength, uint16_t **odata, int *ostride) {
  VC2DECODER_BEGIN

  decoder->decodeFrame(idata, ilength, odata, ostride);
  return VC2DECODER_OK;

  VC2DECODER_END
}

void vc2decode_destroy(VC2DecoderHandle handle) {
  (void) handle;
  VC2Decoder *decoder = (VC2Decoder *)handle;
  delete decoder;
}

VC2DecoderResult vc2decode_synchronise(VC2DecoderHandle handle, char **idata, int ilength, int skip_aux) {
  VC2DECODER_BEGIN

  return (VC2DecoderResult)decoder->sequenceSynchronise(idata, ilength, (bool)skip_aux);

  VC2DECODER_END
}

VC2DecoderResult vc2decode_decode_one_picture(VC2DecoderHandle handle, char **idata, int ilength, uint16_t **odata, int *ostride, int skip_aux) {
  VC2DECODER_BEGIN

  return (VC2DecoderResult)decoder->sequenceDecodeOnePicture(idata, ilength, odata, ostride, (bool)skip_aux);

  VC2DECODER_END
}

VC2DecoderResult vc2decode_sequence_info(VC2DecoderHandle handle, VC2DecoderSequenceInfo *info)  {
  VC2DECODER_BEGIN

  *info = decoder->getSequenceInfo();
  return VC2DECODER_OK;

  VC2DECODER_END
}

VC2DecoderResult vc2decode_extract_aux(VC2DecoderHandle handle, char **idata, int ilength, uint8_t **odata, int *olength) {
  VC2DECODER_BEGIN

  *olength = decoder->sequenceExtractAux(idata, ilength, odata);
  return VC2DECODER_OK;

  VC2DECODER_END
}
