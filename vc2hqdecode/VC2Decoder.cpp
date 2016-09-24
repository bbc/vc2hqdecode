/*****************************************************************************
 * VC2Decoder.cpp : Main implementation file
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

#include "VC2Decoder.hpp"

#include "../vc2inversetransform_c/invtransform_c.hpp"
#include "../vc2inversetransform_sse4_2/invtransform_sse4_2.hpp"

#include "../vc2inversetransform_c/dequantise_c.hpp"
#include "../vc2inversetransform_sse4_2/dequantise_sse4_2.hpp"

#include "../vc2inversetransform_c/vlc_c.hpp"
#include "../vc2inversetransform_sse4_2/vlc_sse4_2.hpp"

#include <stdexcept>
#include <cstdio>
#include <string.h>

#include "logger.hpp"
#include "stream.hpp"

#include "platform_variant.hpp"

#ifdef DEBUG
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include "debug.hpp"
#endif

#ifdef DEBUG_P_BLOCK
static int DEBUG_P_JOB;
static int DEBUG_P_SLICE_Y;
static int DEBUG_P_SLICE_X;
static int DEBUG_P_SLICE_W;
static int DEBUG_P_SLICE_H;
#endif

uint32_t max_to_active_bits(const uint32_t m) {
	return (32 - __builtin_clz(m));
}


GetInvVTransform          get_invvtransform = NULL;
GetInvHTranform           get_invhtransform = NULL;
GetInvHTransformFinal     get_invhtransformfinal = NULL;

GetDequantiseFunctionFunc getDequantiseFunction = NULL;

GetSliceDecoderFunc       get_slice_decoder = NULL;

static bool HAS_SSE4_2 = false;
static bool HAS_AVX = false;
static bool HAS_AVX2 = false;


void detect_cpu_features() {
  __detect_cpu_features(HAS_SSE4_2, HAS_AVX, HAS_AVX2);

	writelog(LOG_INFO, "Processor Features:");
	if (HAS_SSE4_2)
		writelog(LOG_INFO, "  SSE4.2 [X]");
	else
		writelog(LOG_INFO, "  SSE4.2 [ ]");

	if (HAS_AVX)
		writelog(LOG_INFO, "  AVX    [X]");
	else
		writelog(LOG_INFO, "  AVX    [ ]");

	if (HAS_AVX2)
		writelog(LOG_INFO, "  AVX2   [X]");
	else
		writelog(LOG_INFO, "  AVX2   [ ]");

	get_invvtransform = get_invvtransform_c;
	get_invhtransform = get_invhtransform_c;
	get_invhtransformfinal = get_invhtransformfinal_c;

	getDequantiseFunction = getDequantiseFunction_c;

	get_slice_decoder = get_slice_decoder_c;

#ifndef NO_SSE4_2
	if (HAS_SSE4_2) {
		get_invvtransform = get_invvtransform_sse4_2;
		get_invhtransform = get_invhtransform_sse4_2;
		get_invhtransformfinal = get_invhtransformfinal_sse4_2;

		getDequantiseFunction = getDequantiseFunction_sse4_2;
		get_slice_decoder = get_slice_decoder_sse4_2;
	}
#endif
}

#ifdef DEBUG_P_BLOCK
void __debug_print_slice(JobData *job, int sample_size) {
  if (sample_size == 2) {
    for (int y = DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H; y < DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H + DEBUG_P_SLICE_H; y++) {
      int16_t *D = &job->video_data[DEBUG_P_COMP]->as<int16_t>()[y*job->video_data[DEBUG_P_COMP]->stride + DEBUG_P_SLICE_X*DEBUG_P_SLICE_W];
      printf("  ");
      for (int x = 0; x < DEBUG_P_SLICE_W; x++)
        printf("%+6d ", D[x]);
      printf("\n");
    }
  } else if (sample_size == 4) {
    for (int y = DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H; y < DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H + DEBUG_P_SLICE_H; y++) {
      int32_t *D = &job->video_data[DEBUG_P_COMP]->as<int32_t>()[y*job->video_data[DEBUG_P_COMP]->stride + DEBUG_P_SLICE_X*DEBUG_P_SLICE_W];
      printf("  ");
      for (int x = 0; x < DEBUG_P_SLICE_W; x++)
        printf("%+6d ", D[x]);
      printf("\n");
    }
  } else {
    throw;
  }
}
#endif /* DEBUG_P_BLOCK */

VC2DecoderSequenceResult VC2Decoder::sequenceSynchronise(char **_idata, int ilength, bool skip_aux) {
	VC2DecoderParseSegment pi;

	char *idata = *_idata;
	int offset = 0;

	while (offset < ilength) {
		try {
			pi = parse_info(&idata[offset]);
		}
		catch (VC2DecoderResult &r) {
			if (r == VC2DECODER_NOTPARSEINFO) {
				offset++;
				continue;
			}
			else
				throw;
		}
		switch (pi.parse_code) {
		case VC2DECODER_PARSE_CODE_SEQUENCE_HEADER:
			break;

		case VC2DECODER_PARSE_CODE_AUXILIARY_DATA:
			if (!skip_aux) {
				*_idata = &idata[offset];
				return VC2DECODER_AUXILIARY;
			}
		default:
			if (pi.next_header == NULL) {
				offset += 13;
			}
			else {
				offset = pi.next_header - idata;
			}
			continue;
		}
		break;
	}

	if (offset >= ilength) {
		return VC2DECODER_EOS;
	}

	parseSeqHeader(pi.data);

	*_idata = pi.next_header;

	return VC2DECODER_RECONFIGURED;
}

char *VC2Decoder::FindNextParseInfo(char *_idata, int ilength) {
	VC2DecoderParseSegment pi;

	char *idata = _idata;
	int offset = 0;

	while (offset < ilength) {
		try {
			pi = parse_info(&idata[offset]);
		}
		catch (VC2DecoderResult &r) {
			if (r == VC2DECODER_NOTPARSEINFO) {
				offset++;
				continue;
			}
			else
				throw;
		}
		break;
	}

	if (offset >= ilength) {
		return NULL;
	}

	return idata + offset;
}

VC2DecoderSequenceResult VC2Decoder::sequenceDecodeOnePicture(char **_idata, int ilength, uint16_t **odata, int *ostride, bool skip_aux) {
	VC2DecoderParseSegment pi;
	char *idata = *_idata;
	char *iend = *_idata + ilength;

	try {
		while (idata < *(_idata)+ilength) {
			pi = parse_info(idata);
			if ((uint64_t)idata >(uint64_t)iend) {
				writelog(LOG_ERROR, "%s:%d:  Data Unit is off end of input data\n", __FILE__, __LINE__);
				throw VC2DECODER_CODEROVERRUN;
			}
			switch (pi.parse_code) {
			case VC2DECODER_PARSE_CODE_SEQUENCE_HEADER:
				if (parseSeqHeader(pi.data)) {
					*_idata = pi.next_header;
					return VC2DECODER_RECONFIGURED;
				}
				break;

			case VC2DECODER_PARSE_CODE_END_OF_SEQUENCE:
				*_idata = idata + 13;
				return VC2DECODER_EOS;

			case VC2DECODER_PARSE_CODE_AUXILIARY_DATA:
				if (!skip_aux) {
					*_idata = idata;
					return VC2DECODER_AUXILIARY;
				}
			case VC2DECODER_PARSE_CODE_PADDING_DATA:
				break;

			case VC2DECODER_PARSE_CODE_CORE_PICTURE_AC:
			case VC2DECODER_PARSE_CODE_CORE_PICTURE_VLC:
			case VC2DECODER_PARSE_CODE_LD_PICTURE:
				*_idata = pi.next_header;
				return VC2DECODER_INVALID_PICTURE;

			case VC2DECODER_PARSE_CODE_HQ_PICTURE:
			{
				uint64_t length = decodeFrame(pi.data, iend - pi.data, odata, ostride);
				if (pi.next_header != NULL) {
					*_idata = pi.next_header;
				}
				else {
					*_idata = FindNextParseInfo(idata + length, iend - (idata + length));
				}
			}
			return VC2DECODER_PICTURE;

			default:
				writelog(LOG_WARN, "%s:%d:  Unknown parse code 0x%02x\n", __FILE__, __LINE__, pi.parse_code);
				break;
			}

			if (pi.next_header == NULL) {
				*_idata = idata + 13;
				return VC2DECODER_EOS;
			}

			idata = pi.next_header;
		}
	}
	catch (VC2DecoderResult &r) {
		if (r == VC2DECODER_NOTPARSEINFO) {
			writelog(LOG_ERROR, "%s:%d:  No Parse Info Header Where One was Expected\n", __FILE__, __LINE__);
			throw VC2DECODER_BADSTREAM;
		}
		throw;
	}

	writelog(LOG_WARN, "%s:%d: Premature end of stream\n", __FILE__, __LINE__);
	*_idata += ilength;
	return VC2DECODER_EOS;
}

int VC2Decoder::sequenceExtractAux(char **_idata, int ilength, uint8_t **odata) {
	VC2DecoderParseSegment pi;
	char *idata = *_idata;

	if (ilength < 13)
		throw VC2DECODER_CODEROVERRUN;

	pi = parse_info(idata);

	if (pi.parse_code != VC2DECODER_PARSE_CODE_AUXILIARY_DATA) {
		throw VC2DECODER_NOTPARSEINFO;
	}

	if (pi.next_header >= (*_idata) + ilength)
		throw VC2DECODER_CODEROVERRUN;

	*odata = (uint8_t *)pi.data;
	*_idata = pi.next_header;

	return pi.next_header - pi.data;
}

bool VC2Decoder::parseSeqHeader(char *_idata) {
#define EXPECT_VAL(N) { uint32_t d = read_uint(idata, bits);\
    if (d != (N)) {\
      writelog(LOG_WARN, "%s:%d:  Expected %d, got %d when parsing sequence header\n", __FILE__, __LINE__, (N), d); \
    }\
  }

	uint8_t *idata = (uint8_t *)_idata;
	if ((!mSeqHeaderEncoded) || mSeqHeaderEncodedLength == 0 || (memcmp(mSeqHeaderEncoded, idata, mSeqHeaderEncodedLength) != 0)) {
		writelog(LOG_INFO, "Processing Sequence Header");
		int bits = 7;
		EXPECT_VAL(2);
		EXPECT_VAL(0);
		EXPECT_VAL(3);
		int level = read_uint(idata, bits);
		if (level != 3 && level != 6) {
			writelog(LOG_WARN, "%s:%d: Expected 3 or 6, got %d when reading level\n", __FILE__, __LINE__, level);
		}

		VC2DecoderParamsInternal params;
		params = mParams;

		params.video_format.base_video_format = read_uint(idata, bits);

		params.video_format.custom_dimensions_flag = read_bool(idata, bits);
		if (params.video_format.custom_dimensions_flag) {
			params.video_format.frame_width = read_uint(idata, bits);
			params.video_format.frame_height = read_uint(idata, bits);
		}

		params.video_format.custom_color_diff_format_flag = read_bool(idata, bits);
		if (params.video_format.custom_color_diff_format_flag) {
			params.video_format.color_diff_format_index = read_uint(idata, bits);
		}

		params.video_format.custom_scan_format_flag = read_bool(idata, bits);
		if (params.video_format.custom_scan_format_flag) {
			params.video_format.source_sampling = read_uint(idata, bits);
		}

		params.video_format.custom_frame_rate_flag = read_bool(idata, bits);
		if (params.video_format.custom_frame_rate_flag) {
			params.video_format.frame_rate_index = read_uint(idata, bits);
			if (params.video_format.frame_rate_index == 0) {
				params.video_format.frame_rate_numer = read_uint(idata, bits);
				params.video_format.frame_rate_denom = read_uint(idata, bits);
			}
		}

		params.video_format.custom_pixel_aspect_ratio_flag = read_bool(idata, bits);
		if (params.video_format.custom_pixel_aspect_ratio_flag) {
			params.video_format.pixel_aspect_ratio_index = read_uint(idata, bits);
			if (params.video_format.pixel_aspect_ratio_index == 0) {
				params.video_format.pixel_aspect_ratio_numer = read_uint(idata, bits);
				params.video_format.pixel_aspect_ratio_denom = read_uint(idata, bits);
			}
		}

		params.video_format.custom_clean_area_flag = read_bool(idata, bits);
		if (params.video_format.custom_clean_area_flag) {
			params.video_format.clean_width = read_uint(idata, bits);
			params.video_format.clean_height = read_uint(idata, bits);
			params.video_format.left_offset = read_uint(idata, bits);
			params.video_format.top_offset = read_uint(idata, bits);
		}

		params.video_format.custom_signal_range_flag = read_bool(idata, bits);
		if (params.video_format.custom_signal_range_flag) {
			params.video_format.signal_range_index = read_uint(idata, bits);
			if (params.video_format.signal_range_index == 0) {
				params.video_format.luma_offset = read_uint(idata, bits);
				params.video_format.luma_excursion = read_uint(idata, bits);
				params.video_format.color_diff_offset = read_uint(idata, bits);
				params.video_format.color_diff_excursion = read_uint(idata, bits);
			}
		}

		params.video_format.custom_color_spec_flag = read_bool(idata, bits);
		if (params.video_format.custom_color_spec_flag) {
			params.video_format.color_spec_index = read_uint(idata, bits);
			if (params.video_format.color_spec_index == 0) {
				params.video_format.custom_color_primaries_flag = read_bool(idata, bits);
				if (params.video_format.custom_color_primaries_flag) {
					params.video_format.color_primaries_index = read_uint(idata, bits);
				}

				params.video_format.custom_color_matrix_flag = read_bool(idata, bits);
				if (params.video_format.custom_color_matrix_flag) {
					params.video_format.color_matrix_index = read_uint(idata, bits);
				}

				params.video_format.custom_transfer_function_flag = read_bool(idata, bits);
				if (params.video_format.custom_transfer_function_flag) {
					params.video_format.transfer_function_index = read_uint(idata, bits);
				}
			}
		}

		uint32_t picture_coding_mode = read_uint(idata, bits);
		mInterlaced = (picture_coding_mode != 0);

		mParams = params;
		mConfigured = false;
		setVideoFormat(mParams);

		byte_align(idata, bits);
		if (mSeqHeaderEncoded)
			delete[] mSeqHeaderEncoded;
		mSeqHeaderEncodedLength = idata - (uint8_t *)_idata;
		mSeqHeaderEncoded = new uint8_t[mSeqHeaderEncodedLength];
		memcpy(mSeqHeaderEncoded, _idata, mSeqHeaderEncodedLength);

		mSequenceInfo.video_format = params.video_format;
		mSequenceInfo.picture_coding_mode = picture_coding_mode;
		mSequenceInfo.sequence_headers_seen++;
		return true;
	}
	return false;
}

void VC2Decoder::setUserParams(VC2DecoderParamsUser &params) {
	mParams.threads = params.threads;
	mParams.numa_first_node = params.numa_first_node;

	mParams.colourise = params.colourise_quantiser || params.colourise_padding || params.colourise_unpadded;
	mParams.colourise_quantiser = params.colourise_quantiser;
	mParams.colourise_padding = params.colourise_padding;
	mParams.colourise_unpadded = params.colourise_unpadded;

	mParams.partial_decode = false;

	if (params.partial_decode) {
		mParams.partial_decode = true;
		mParams.partial_decode_offset_x = params.partial_decode_offset_x;
		mParams.partial_decode_offset_y = params.partial_decode_offset_y;
		mParams.partial_decode_width = params.partial_decode_width;
		mParams.partial_decode_height = params.partial_decode_height;
	}

	if (mConfigured)
		setParams(mParams);
}

void VC2Decoder::setVideoFormat(VC2DecoderParamsInternal &params) {
	mVideoFormat = vc2::preset_formats[params.video_format.base_video_format];

	if (params.video_format.custom_dimensions_flag) {
		mVideoFormat.frame_width = params.video_format.frame_width;
		mVideoFormat.frame_height = params.video_format.frame_height;
	}

	if (params.video_format.custom_color_diff_format_flag) {
		mVideoFormat.color_diff_format_index = params.video_format.color_diff_format_index;
	}

	if (params.video_format.custom_scan_format_flag) {
		mVideoFormat.source_sampling = params.video_format.source_sampling;
	}

	if (params.video_format.custom_frame_rate_flag) {
		if (params.video_format.frame_rate_index == 0) {
			mVideoFormat.frame_rate_numer = params.video_format.frame_rate_numer;
			mVideoFormat.frame_rate_denom = params.video_format.frame_rate_denom;
		}
		else {
			mVideoFormat.frame_rate_numer = vc2::preset_framerates[params.video_format.frame_rate_index].numer;
			mVideoFormat.frame_rate_denom = vc2::preset_framerates[params.video_format.frame_rate_index].denom;
		}
	}

	if (params.video_format.custom_pixel_aspect_ratio_flag) {
		if (params.video_format.pixel_aspect_ratio_index == 0) {
			mVideoFormat.pixel_aspect_ratio_numer = params.video_format.pixel_aspect_ratio_numer;
			mVideoFormat.pixel_aspect_ratio_denom = params.video_format.pixel_aspect_ratio_denom;
		}
		else {
			mVideoFormat.pixel_aspect_ratio_numer = vc2::preset_pixel_aspect_ratios[params.video_format.pixel_aspect_ratio_index][0];
			mVideoFormat.pixel_aspect_ratio_denom = vc2::preset_pixel_aspect_ratios[params.video_format.pixel_aspect_ratio_index][1];
		}
	}

	if (params.video_format.custom_clean_area_flag) {
		mVideoFormat.clean_width = params.video_format.clean_width;
		mVideoFormat.clean_height = params.video_format.clean_height;
		mVideoFormat.left_offset = params.video_format.left_offset;
		mVideoFormat.top_offset = params.video_format.top_offset;
	}

	if (params.video_format.custom_signal_range_flag) {
		if (params.video_format.signal_range_index != 0) {
			mVideoFormat.luma_offset = vc2::preset_signal_ranges[params.video_format.signal_range_index].luma_offset;
			mVideoFormat.luma_excursion = vc2::preset_signal_ranges[params.video_format.signal_range_index].luma_excursion;
			mVideoFormat.luma_bytes_per_sample = vc2::preset_signal_ranges[params.video_format.signal_range_index].luma_bytes_per_sample;
			mVideoFormat.luma_active_bits = vc2::preset_signal_ranges[params.video_format.signal_range_index].luma_active_bits;
			mVideoFormat.color_diff_offset = vc2::preset_signal_ranges[params.video_format.signal_range_index].color_diff_offset;
			mVideoFormat.color_diff_excursion = vc2::preset_signal_ranges[params.video_format.signal_range_index].color_diff_excursion;
			mVideoFormat.color_diff_bytes_per_sample = vc2::preset_signal_ranges[params.video_format.signal_range_index].color_diff_bytes_per_sample;
			mVideoFormat.color_diff_active_bits = vc2::preset_signal_ranges[params.video_format.signal_range_index].color_diff_active_bits;
		}
		else {
			mVideoFormat.luma_offset = params.video_format.luma_offset;
			mVideoFormat.luma_excursion = params.video_format.luma_excursion;
			mVideoFormat.luma_active_bits = max_to_active_bits(params.video_format.luma_excursion + params.video_format.luma_offset);
			mVideoFormat.luma_bytes_per_sample = (mVideoFormat.luma_active_bits > 8) ? 2 : 1;
			mVideoFormat.color_diff_offset = params.video_format.color_diff_offset;
			mVideoFormat.color_diff_excursion = params.video_format.color_diff_excursion;
			mVideoFormat.color_diff_active_bits = max_to_active_bits(params.video_format.color_diff_excursion / 2 + params.video_format.color_diff_offset);
			mVideoFormat.color_diff_bytes_per_sample = (mVideoFormat.color_diff_active_bits > 8) ? 2 : 1;
		}
	}

	if (params.video_format.custom_color_spec_flag) {
		if (params.video_format.color_spec_index == 0) {
			if (params.video_format.custom_color_primaries_flag) {
				mVideoFormat.color_primaries = params.video_format.color_primaries_index;
			}

			if (params.video_format.custom_color_matrix_flag) {
				mVideoFormat.color_matrix = params.video_format.color_matrix_index;
			}

			if (params.video_format.custom_transfer_function_flag) {
				mVideoFormat.transfer_function = params.video_format.transfer_function_index;
			}
		}
		else {
			mVideoFormat.color_primaries = vc2::preset_color_specs[params.video_format.color_spec_index].color_primaries;
			mVideoFormat.color_matrix = vc2::preset_color_specs[params.video_format.color_spec_index].color_matrix;
			mVideoFormat.transfer_function = vc2::preset_color_specs[params.video_format.color_spec_index].transfer_function;
		}
	}

	if ((mVideoFormat.luma_active_bits != 10 && mVideoFormat.luma_active_bits != 12) || (mVideoFormat.color_diff_active_bits != 10 && mVideoFormat.color_diff_active_bits != 12) ||
		//      mVideoFormat.frame_width != 1920 || mVideoFormat.frame_height != 1080 ||
		mVideoFormat.color_diff_format_index != VC2DECODER_CDS_422) {
		writelog(LOG_ERROR, "%s:%d:  Frame geometry not supported, only 4:2:2 10-bit or 12-bit is supported currently.", __FILE__, __LINE__);
		throw VC2DECODER_NOTIMPLEMENTED;
	}

	mOutputFormat.width = mVideoFormat.frame_width;
	mOutputFormat.height = mVideoFormat.frame_height;
	if (mVideoFormat.luma_active_bits == 10)
		mOutputFormat.signal_range = VC2DECODER_PSR_10BITVID;
	else if (mVideoFormat.luma_active_bits == 12)
		mOutputFormat.signal_range = VC2DECODER_PSR_12BITVID;
	mOutputFormat.source_sampling = mVideoFormat.color_diff_format_index;
	mOutputFormat.frame_rate_numer = mVideoFormat.frame_rate_numer;
	mOutputFormat.frame_rate_denom = mVideoFormat.frame_rate_denom;
	mOutputFormat.interlaced = mInterlaced;

	if (mParams.partial_decode) {
		mOutputFormat.width = mParams.partial_decode_width;
		mOutputFormat.height = mParams.partial_decode_height;
	}

	writelog(LOG_INFO, "Configuring for %d x %d", mOutputFormat.width, mOutputFormat.height);
}

void VC2Decoder::setParams(VC2DecoderParamsInternal &params) {
	int sample_size = 2;

	if (params.transform_params.wavelet_index == VC2DECODER_WFT_FIDELITY ||
		params.transform_params.wavelet_index == VC2DECODER_WFT_DAUBECHIES_9_7) {
		sample_size = 4;
	}

	mSlicesX = params.transform_params.slices_x;
	mSlicesY = params.transform_params.slices_y;


	if (mJobs) {
		for (int i = 0; i < mJobsX*mJobsY; i++) {
			delete mJobs[i];
		}
		delete[] mJobs;
	}

	if (mSliceJobLUTX) {
		delete[] mSliceJobLUTX;
	}

	if (mSliceJobLUTY) {
		delete[] mSliceJobLUTY;
	}

#ifndef DEBUG_ONE_JOB
	int n_threads = params.threads;
	int n_jobs = 1;
	for (n_jobs = 1; n_jobs < 4 * n_threads; n_jobs <<= 1);
#else
	int n_threads = 1;
	int n_jobs = 1;
#endif

	mJobs = new JobData*[n_jobs];
	writelog(LOG_INFO, "Configuring for %d threads, %d jobs", n_threads, n_jobs);

	mWidth = mVideoFormat.frame_width;
	mHeight = mVideoFormat.frame_height;
	if (mInterlaced) mHeight /= 2;

  int padded_width  = (mWidth  + ( 1 << params.transform_params.wavelet_depth ) - 1)/( 1 << params.transform_params.wavelet_depth )*( 1 << params.transform_params.wavelet_depth );
  int padded_height = (mHeight  + ( 1 << params.transform_params.wavelet_depth ) - 1)/( 1 << params.transform_params.wavelet_depth )*( 1 << params.transform_params.wavelet_depth );

  if ((padded_width%params.transform_params.slices_x != 0) ||
      (padded_height%params.transform_params.slices_y != 0)) {
    writelog(LOG_ERROR, "%s:%d:  The decoder does not currently support streams with variable slice geometries.\n", __FILE__, __LINE__);
    throw VC2DECODER_NOTIMPLEMENTED;
  }

	int slice_width = (padded_width / params.transform_params.slices_x);
	int slice_height = (padded_height / params.transform_params.slices_y);

	{
#ifdef DEBUG_P_BLOCK
		DEBUG_P_JOB = 0;
		DEBUG_P_SLICE_Y = DEBUG_P_BLOCK_Y;
		DEBUG_P_SLICE_X = DEBUG_P_BLOCK_X;
#endif

		switch (n_jobs) {
		case 1:
			mJobsX = 1;
			mJobsY = 1;
			break;
		case 2:
			mJobsX = 2;
			mJobsY = 1;
			break;
		case 4:
			mJobsX = 2;
			mJobsY = 2;
			break;
		case 8:
			mJobsX = 4;
			mJobsY = 2;
			break;
		case 16:
			mJobsX = 4;
			mJobsY = 4;
			break;
		case 32:
			mJobsX = 8;
			mJobsY = 4;
			break;
		case 64:
			mJobsX = 8;
			mJobsY = 8;
			break;
		default:
			writelog(LOG_ERROR, "%s:%d:  Invalid number of jobs, only 1, 2, 4, 8, 16, 32, and 64 are currently supported\n", __FILE__, __LINE__);
			throw VC2DECODER_NOTIMPLEMENTED;
		}
		mSliceJobLUTX = new uint8_t[mSlicesX];
		mSliceJobLUTY = new uint8_t[mSlicesY];

		int slice_skip_x = 0;
		int slice_skip_y = 0;
		int slices_in_output_x = mSlicesX;
		int slices_in_output_y = mSlicesY;
		int pixel_margin_pre_x = 0;
		int pixel_margin_pre_y = 0;
		int pixel_margin_post_x = (slice_width - (mWidth%slice_width)) % slice_width;
		int pixel_margin_post_y = (slice_height - (mHeight%slice_height)) % slice_height;

		if (mParams.partial_decode) {
			slice_skip_x = mParams.partial_decode_offset_x / slice_width;
			slice_skip_y = mParams.partial_decode_offset_y / slice_height;
			slices_in_output_x = mSlicesX - slice_skip_x - ((mWidth - mParams.partial_decode_offset_x - mParams.partial_decode_width) / slice_width);
			slices_in_output_y = mSlicesY - slice_skip_y - ((mHeight - mParams.partial_decode_offset_y - mParams.partial_decode_height) / slice_height);

			pixel_margin_pre_x = mParams.partial_decode_offset_x%slice_width;
			pixel_margin_pre_y = mParams.partial_decode_offset_y%slice_height;

			pixel_margin_post_x = (mWidth - mParams.partial_decode_offset_x - mParams.partial_decode_width) % slice_width;
			pixel_margin_post_y = (mHeight - mParams.partial_decode_offset_y - mParams.partial_decode_height) % slice_height;
		}


		int spj_x = (slices_in_output_x + mJobsX - 1) / mJobsX;
		int spj_y = (slices_in_output_y + mJobsY - 1) / mJobsY;

		mOverlapX = 32 / slice_width;

		for (int y = 0; y < mJobsY; y++) {
			int s_y = (y < mJobsY - 1) ? spj_y : slices_in_output_y - y*spj_y;
			int pad_yz = ((y < mJobsY - 1) ? 1 : 0);
			int pad_ya = ((y > 0) ? 1 : 0);

			int PADY_PRE = ((y > 0) ? (slice_height) : (pixel_margin_pre_y));
			int PADY_POST = ((y < mJobsY - 1) ? 0 : pixel_margin_post_y + pixel_margin_pre_y);
			for (int x = 0; x < mJobsX; x++) {
				int s_x = (x < mJobsX - 1) ? spj_x : slices_in_output_x - x*spj_x;
				int pad_xz = ((x < mJobsX - 1) ? mOverlapX : 0);
				int pad_xa = ((x > 0) ? mOverlapX : 0);

				int PADX_PRE = ((x > 0) ? (mOverlapX*slice_width) : (pixel_margin_pre_x));
				int PADX_POST = ((x < mJobsX - 1) ? 0 : pixel_margin_post_x + pixel_margin_pre_x);
				mJobs[y*mJobsX + x] = new JobData(y*mJobsX + x,
					(pad_xa + s_x + pad_xz)*slice_width,
					(pad_ya + s_y + pad_yz)*slice_height,
					mVideoFormat.frame_height,
					(pad_xa + s_x + pad_xz), (pad_ya + s_y + pad_yz),
					PADX_PRE,
					PADY_PRE,
					s_x*slice_width - PADX_POST, s_y*slice_height - PADY_POST,
					(x*spj_x)*slice_width, (y*spj_y)*slice_height,
					x*spj_x - pad_xa, y*spj_y - pad_ya,
					sample_size);

#ifdef DEBUG_P_BLOCK
				if (DEBUG_P_BLOCK_Y >= spj_y*y && DEBUG_P_BLOCK_Y < spj_y*y + s_y &&
					DEBUG_P_BLOCK_X >= spj_x*x && DEBUG_P_BLOCK_X < spj_x*x + s_x) {
					DEBUG_P_JOB = y*mJobsX + x;
					DEBUG_P_SLICE_Y = DEBUG_P_BLOCK_Y - spj_y*y + pad_ya;
					DEBUG_P_SLICE_X = DEBUG_P_BLOCK_X - spj_x*x + pad_xa;
				}
#endif
			}
		}

		{
			int Y = 0;
			for (int y = 0; y < slice_skip_y; y++) {
				mSliceJobLUTY[Y++] = 0x00;
			}
			if (mJobsY == 1) {
				for (int y = 0; y < mJobs[0]->slices_y; y++) {
					mSliceJobLUTY[Y++] = 0x80;
				}
			}
			else {
				for (int y = 0; y < mJobs[0]->slices_y - 2; y++) {
					mSliceJobLUTY[Y++] = 0x80;
				}
				mSliceJobLUTY[Y++] = 0xC0;
				mSliceJobLUTY[Y++] = 0xC0;
				for (int jy = 1; jy < mJobsY - 1; jy++) {
					for (int y = 2; y < mJobs[jy*mJobsX]->slices_y - 2; y++) {
						mSliceJobLUTY[Y++] = 0x80 | (jy & 0x3F);
					}
					mSliceJobLUTY[Y++] = 0xC0 | (jy & 0x3F);
					mSliceJobLUTY[Y++] = 0xC0 | (jy & 0x3F);
				}
				for (int y = 2; y < mJobs[(mJobsY - 1)*mJobsX]->slices_y; y++) {
					mSliceJobLUTY[Y++] = 0x80 | ((mJobsY - 1) & 0x3F);
				}
			}
			while (Y < mSlicesY) {
				mSliceJobLUTY[Y++] = 0x00;
			}
		}

		{
			int X = 0;
			for (int x = 0; x < slice_skip_x; x++) {
				mSliceJobLUTX[X++] = 0x00;
			}
			if (mJobsX == 1) {
				for (int x = 0; x < mJobs[0]->slices_x; x++) {
					mSliceJobLUTX[X++] = 0x80;
				}
			}
			else {
				for (int x = 0; x < mJobs[0]->slices_x - 2 * mOverlapX; x++) {
					mSliceJobLUTX[X++] = 0x80;
				}
				for (int x = 0; x < 2 * mOverlapX; x++) {
					mSliceJobLUTX[X++] = 0xC0;
				}
				for (int jx = 1; jx < mJobsX - 1; jx++) {
					for (int x = 2 * mOverlapX; x < mJobs[jx]->slices_x - 2 * mOverlapX; x++) {
						mSliceJobLUTX[X++] = 0x80 | (jx & 0x3F);
					}
					for (int x = 0; x < 2 * mOverlapX; x++) {
						mSliceJobLUTX[X++] = 0xC0 | (jx & 0x3F);
					}
				}
				for (int x = 2 * mOverlapX; x < mJobs[(mJobsX - 1)]->slices_x; x++) {
					mSliceJobLUTX[X++] = 0x80 | ((mJobsX - 1) & 0x3F);
				}
			}
			while (X < mSlicesX) {
				mSliceJobLUTX[X++] = 0x00;
			}
		}
	}

	if (mMatrices)
		delete_matrices(mMatrices);

	mMatrices = quantisation_matrices(params.transform_params.wavelet_index, params.transform_params.wavelet_depth, 256);

	if (mPool) {
		mPool->stop();
		delete mPool;
		mPool = NULL;
	}

	mThreads = n_threads;
	if (mThreads > 1)
		mPool = new ThreadPool(mThreads, params.numa_first_node);

	mParams = params;

	if (transforms_h)
		delete[] transforms_h;
	transforms_h = new InplaceTransform[params.transform_params.wavelet_depth - 1];
	for (int l = 0; l < (int)params.transform_params.wavelet_depth - 1; l++)
		transforms_h[l] = get_invhtransform(params.transform_params.wavelet_index, l, params.transform_params.wavelet_depth, sample_size);

	int active_bits;
	if (mOutputFormat.signal_range == VC2DECODER_PSR_10BITVID)
		active_bits = 10;
	else if (mOutputFormat.signal_range == VC2DECODER_PSR_12BITVID)
		active_bits = 12;
	transforms_final = get_invhtransformfinal(params.transform_params.wavelet_index, active_bits, sample_size);

	if (transforms_v)
		delete[] transforms_v;
	transforms_v = new InplaceTransform[params.transform_params.wavelet_depth];
	for (int l = 0; l < (int)params.transform_params.wavelet_depth; l++)
		transforms_v[l] = get_invvtransform(params.transform_params.wavelet_index, l, params.transform_params.wavelet_depth, sample_size);

	mDequant[0] = getDequantiseFunction(slice_width, slice_height, mParams.transform_params.wavelet_depth, sample_size);
	mDequant[1] = getDequantiseFunction(slice_width / 2, slice_height, mParams.transform_params.wavelet_depth, sample_size);
	mDequant[2] = getDequantiseFunction(slice_width / 2, slice_height, mParams.transform_params.wavelet_depth, sample_size);

	mSliceDecoder = get_slice_decoder(sample_size);

  mSampleSize = sample_size;

#ifdef DEBUG_P_BLOCK
	DEBUG_P_SLICE_W = (DEBUG_P_COMP == 0) ? slice_width : slice_width / 2;
	DEBUG_P_SLICE_H = slice_height;
#endif
}

int VC2Decoder::processTransformParams(uint8_t *_idata, int ilength) {
	(void)ilength;
	uint8_t *idata = (uint8_t *)_idata;

	if (!mConfigured || !mTransformParamsEncoded || mTransformParamsEncodedLength == 0 || memcmp(mTransformParamsEncoded, idata, mTransformParamsEncodedLength)) {
		writelog(LOG_INFO, "Processing Transform Params");
		int bits = 7;
		VC2DecoderTransformParams transform_params;
		transform_params.wavelet_index = read_uint(idata, bits);
		transform_params.wavelet_depth = read_uint(idata, bits);
		transform_params.slices_x = read_uint(idata, bits);
		transform_params.slices_y = read_uint(idata, bits);
		int prefix_bytes = read_uint(idata, bits);
		int slice_size_scalar = read_uint(idata, bits);

		transform_params.custom_quant_matrix_flag = read_bool(idata, bits);
		if (transform_params.custom_quant_matrix_flag) {
			transform_params.quant_matrix_LL = read_uint(idata, bits);
			for (int l = 0; l < (int)transform_params.wavelet_depth - 1; l++) {
				transform_params.quant_matrix_HL[l] = read_uint(idata, bits);
				transform_params.quant_matrix_LH[l] = read_uint(idata, bits);
				transform_params.quant_matrix_HH[l] = read_uint(idata, bits);
			}
		}
		byte_align(idata, bits);

		mTransformParamsEncodedLength = (idata - (uint8_t*)_idata);
		if (mTransformParamsEncoded)
			delete[] mTransformParamsEncoded;
		mTransformParamsEncoded = new uint8_t[mTransformParamsEncodedLength];
		memcpy(mTransformParamsEncoded, _idata, mTransformParamsEncodedLength);

		VC2DecoderParamsInternal params = mParams;
		params.transform_params = transform_params;
		params.slice_size_scalar = slice_size_scalar;
		params.slice_prefix_bytes = prefix_bytes;
		setParams(params);

		mSequenceInfo.transform_params = transform_params;

		mConfigured = true;

#ifdef DEBUG
		{
			printf("--------------------------------------------------------------------\n");
			printf("   Picture Header Stream Data\n");
			printf("--------------------------------------------------------------------\n");
			uint8_t *data = (uint8_t *)&_idata;
			for (int y = 0; y * 16 < mTransformParamsEncodedLength; y++) {
				printf("    ");
				for (int x = 0; x < 16 && y * 16 + x < mTransformParamsEncodedLength; x++) {
					printf("  %02x", data[y * 16 + x]);
				}
				printf("\n");
			}
			printf("--------------------------------------------------------------------\n");
		}
#endif
	}
	else {
		idata += mTransformParamsEncodedLength;
	}

	return idata - _idata;
}

uint64_t VC2Decoder::decodeFrame(char *_idata, int ilength, uint16_t **odata, int *ostride) {
	uint8_t *idata = (uint8_t *)_idata;

	// Start by parsing picture header and transform parameters
	uint32_t picture_number = ((idata[0] << 24) |
		(idata[1] << 16) |
		(idata[2] << 8) |
		(idata[3] << 0));
	mSequenceInfo.last_picture_number = picture_number;
	idata += 4;

	idata += processTransformParams(idata, ilength);

	int preamble = idata - (uint8_t*)_idata;

	// Now decode the frame
	uint64_t length = SliceInput((char *)idata, ilength - preamble, mJobs);

#ifndef DEBUG
	if (mThreads > 1) {
		mPool->ready();
		for (int n = 0; n < mJobsX*mJobsY; n++)
			mPool->post(std::bind(&VC2Decoder::Decode, this, mJobs[n], odata, ostride));
		if (mPool->execute())
			throw VC2DECODER_DECODE_FAILED;
	}
	else
#endif /*DEBUG*/
	{
		for (int n = 0; n < mJobsX*mJobsY; n++)
			Decode(mJobs[n], odata, ostride);
	}
	mSequenceInfo.pictures_decoded++;

	return length;
}

uint64_t VC2Decoder::SliceInput(char *_idata, int ilength, JobData **jobs) {
	int lastlength = mParams.slice_prefix_bytes;
	char *idata = _idata;

	for (int sy = 0; sy < mSlicesY; sy++) {
		if ((mSliceJobLUTY[sy] & 0x80) == 0) {
			for (int sx = 0; sx < mSlicesX; sx++) {
				idata += lastlength + 1;
				int length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
				idata += length;
#ifdef DEBUG_OP_SLICESIZES
				writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
				length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
				idata += length;
#ifdef DEBUG_OP_SLICESIZES
				writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
				lastlength = ((int)(*((uint8_t *)idata++))*mParams.slice_size_scalar) + mParams.slice_prefix_bytes;
#ifdef DEBUG_OP_SLICESIZES
				writelog(LOG_INFO, "  skip   %6d\n", lastlength - mParams.slice_prefix_bytes);
#endif
			}
		}
		else if ((mSliceJobLUTY[sy] & 0xC0) == 0x80) {

			for (int sx = 0; sx < mSlicesX; sx++) {
				if ((mSliceJobLUTX[sx] & 0x80) == 0x00) {
					idata += lastlength + 1;
					int length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
					idata += length;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
					length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
					idata += length;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
					lastlength = ((int)(*((uint8_t *)idata++))*mParams.slice_size_scalar) + mParams.slice_prefix_bytes;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", lastlength - mParams.slice_prefix_bytes);
#endif
				}
				else {

					JobData *job = jobs[(mSliceJobLUTY[sy] & 0x3F)*mJobsX + (mSliceJobLUTX[sx] & 0x3f)];
					const int n = (sy - job->slice_start_y)*job->slices_x + (sx - job->slice_start_x);

					idata += lastlength;
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].qindex = (int)(*(idata++));
					job->coded_slices[n].length[0] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[0]);
#endif

					job->coded_slices[n].data[0] = idata;
					idata += job->coded_slices[n].length[0];
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].length[1] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[1]);
#endif

					job->coded_slices[n].data[1] = idata;
					idata += job->coded_slices[n].length[1];
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].length[2] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[2]);
#endif

					job->coded_slices[n].data[2] = idata;

					lastlength = job->coded_slices[n].length[2] + mParams.slice_prefix_bytes;

					if ((mSliceJobLUTX[sx] & 0xC0) == 0xC0) {
						JobData *njob_x = jobs[(mSliceJobLUTY[sy] & 0x3F)*mJobsX + (mSliceJobLUTX[sx] & 0x3f) + 1];
						const int nn_x = (sy - njob_x->slice_start_y)*njob_x->slices_x + (sx - njob_x->slice_start_x);
						memcpy((char *)&njob_x->coded_slices[nn_x],
							(char *)&job->coded_slices[n],
							sizeof(CodedSlice));
					}
				}
			}

		}
		else {

			for (int sx = 0; sx < mSlicesX; sx++) {
				if ((mSliceJobLUTX[sx] & 0x80) == 0x00) {
					idata += lastlength + 1;
					int length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
					idata += length;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
					length = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;
					idata += length;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", length);
#endif
					lastlength = ((int)(*((uint8_t *)idata++))*mParams.slice_size_scalar) + mParams.slice_prefix_bytes;
#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  skip   %6d\n", lastlength - mParams.slice_prefix_bytes);
#endif
				}
				else {

					JobData *job = jobs[(mSliceJobLUTY[sy] & 0x3F)*mJobsX + (mSliceJobLUTX[sx] & 0x3f)];
					const int n = (sy - job->slice_start_y)*job->slices_x + (sx - job->slice_start_x);

					JobData *njob_y = jobs[((mSliceJobLUTY[sy] & 0x3F) + 1)*mJobsX + (mSliceJobLUTX[sx] & 0x3f)];
					const int nn_y = (sy - njob_y->slice_start_y)*njob_y->slices_x + (sx - njob_y->slice_start_x);

					idata += lastlength;
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].qindex = (int)(*(idata++));
					job->coded_slices[n].length[0] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[0]);
#endif

					job->coded_slices[n].data[0] = idata;
					idata += job->coded_slices[n].length[0];
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].length[1] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[1]);
#endif

					job->coded_slices[n].data[1] = idata;
					idata += job->coded_slices[n].length[1];
					if (idata > _idata + ilength) {
						writelog(LOG_ERROR, "%s:%d:  Coder Overrun: %d > %d\n", __FILE__, __LINE__, (int)(idata - _idata), ilength);
						throw VC2DECODER_CODEROVERRUN;
					}

					job->coded_slices[n].length[2] = (int)(*((uint8_t *)idata++))*mParams.slice_size_scalar;

#ifdef DEBUG_OP_SLICESIZES
					writelog(LOG_INFO, "  %5u  %6d\n", n, job->coded_slices[n].length[2]);
#endif

					job->coded_slices[n].data[2] = idata;

					lastlength = job->coded_slices[n].length[2] + mParams.slice_prefix_bytes;

					memcpy((char *)&njob_y->coded_slices[nn_y],
						(char *)&job->coded_slices[n],
						sizeof(CodedSlice));

					if ((mSliceJobLUTX[sx] & 0xC0) == 0xC0) {
						JobData *njob_x = jobs[(mSliceJobLUTY[sy] & 0x3F)*mJobsX + (mSliceJobLUTX[sx] & 0x3f) + 1];
						const int nn_x = (sy - njob_x->slice_start_y)*njob_x->slices_x + (sx - njob_x->slice_start_x);
						JobData *njob_xy = jobs[((mSliceJobLUTY[sy] & 0x3F) + 1)*mJobsX + (mSliceJobLUTX[sx] & 0x3f) + 1];
						const int nn_xy = (sy - njob_xy->slice_start_y)*njob_xy->slices_x + (sx - njob_xy->slice_start_x);
						memcpy((char *)&njob_x->coded_slices[nn_x],
							(char *)&job->coded_slices[n],
							sizeof(CodedSlice));
						memcpy((char *)&njob_xy->coded_slices[nn_xy],
							(char *)&job->coded_slices[n],
							sizeof(CodedSlice));
					}
				}
			}
		}
	}

#ifdef DEBUG_OP_LENGTHS
	{
		int f = open("length_dump.raw", O_WRONLY | O_CREAT | O_TRUNC, 00777);
		for (int j = 0; j < mJobsX*mJobsY; j++) {
			for (int n = 0; n < jobs[j]->slices_y*jobs[j]->slices_x; n++) {
				write(f, jobs[j]->coded_slices[n].length, sizeof(int) * 3);
			}
		}
		close(f);
	}
#endif /* DEBUG_OP_LENGTHS */

#ifdef DEBUG_P_BLOCK
	CodedSlice *slice = &mJobs[DEBUG_P_JOB]->coded_slices[DEBUG_P_SLICE_Y*mJobs[DEBUG_P_JOB]->slices_x + DEBUG_P_SLICE_X];
	{
		printf("-----------------------------------------------------------------\n");
		printf("  Deserialised\n");
		printf("-----------------------------------------------------------------\n");
		printf("    %02x (%d)\n", slice->qindex, slice->qindex);
		for (int c = 0; c < 3; c++) {
			printf("    %02x (%d)\n", slice->length[c], slice->length[c]);
			for (int y = 0; y < (slice->length[c] + 15) / 16; y++) {
				printf("    ");
				for (int x = 0; x < 16 && y * 16 + x < slice->length[c]; x++) {
					printf("%02x ", ((uint8_t *)(slice->data[c]))[y * 16 + x]);
				}
				printf("\n");
			}
		}
		printf("-----------------------------------------------------------------\n");
	}
#endif

	return (((uint64_t)idata) + 1 + lastlength) - ((uint64_t)_idata);
}

void VC2Decoder::Decode(JobData *job, uint16_t **_odata, int *_ostride) {
	int slice_width = (mWidth + mParams.transform_params.slices_x - 1) / mSlicesX;
	int slice_height = (mHeight + mParams.transform_params.slices_y - 1) / mSlicesY;

	mSliceDecoder(mMatrices,
		job->coded_slices,
		job->decoded_slice,
		job->slices_x, job->slices_y,
		job->video_data,
		slice_width,
		slice_height,
		mParams.transform_params.wavelet_depth,
		mDequant);

#ifdef DEBUG_OP_TRANSFORMED
	{
		std::stringstream ss;
		ss << "transformed_dump_" << job->number << ".raw";
		int f = open(ss.str().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 00777);
		for (int c = 0; c < 3; c++) {
			for (int y = 0; y < job->video_data[c]->height; y++) {
				ssize_t s = write(f, (char *)&job->video_data[c]->data[y*job->video_data[c]->stride], job->video_data[c]->width*sizeof(int32_t));
				if (s < 0) {
					throw std::runtime_error("File Writing Error");
				}
			}
		}
	}
#endif /* DEBUG_OP_TRANSFORMED */

	job->ostride[0] = _ostride[0];
	job->ostride[1] = _ostride[1];
	job->ostride[2] = _ostride[2];

	job->odata[0] = (char *)(_odata[0] + job->ostride[0] * job->target_y[0] + job->target_x[0]);
	job->odata[1] = (char *)(_odata[1] + job->target_y[1] * job->ostride[1] + job->target_x[1]);
	job->odata[2] = (char *)(_odata[2] + job->target_y[2] * job->ostride[2] + job->target_x[2]);

#ifdef DEBUG_P_BLOCK
  if (job->number == DEBUG_P_JOB) {
    printf("-----------------------------------------------------------------\n");
    printf("Dequantised\n");
    printf("-----------------------------------------------------------------\n");
    __debug_print_slice(job, mSampleSize);
    printf("-----------------------------------------------------------------\n");
  }
#endif
	int C = mParams.colourise ? 1 : 3;
	for (int c = 0; c < C; c++) {
		int l;
		for (l = 0; l < (int)mParams.transform_params.wavelet_depth - 1; l++) {
			transforms_v[l](job->video_data[c]->data,
				job->video_data[c]->stride,
				job->video_data[c]->width,
				job->video_data[c]->height);

#ifdef DEBUG_P_BLOCK
      if (job->number == DEBUG_P_JOB && c == DEBUG_P_COMP) {
        printf("-----------------------------------------------------------------\n");
        printf("Transform V%d\n", l);
        printf("-----------------------------------------------------------------\n");
        __debug_print_slice(job, mSampleSize);
        printf("-----------------------------------------------------------------\n");
      }
#endif

			transforms_h[l](job->video_data[c]->data,
				job->video_data[c]->stride,
				job->video_data[c]->width,
				job->video_data[c]->height);

#ifdef DEBUG_P_BLOCK
      if (job->number == DEBUG_P_JOB && c == DEBUG_P_COMP) {
        printf("-----------------------------------------------------------------\n");
        printf("Transform H%d\n", l);
        printf("-----------------------------------------------------------------\n");
        __debug_print_slice(job, mSampleSize);
        printf("-----------------------------------------------------------------\n");
      }
#endif
		}

		{
			transforms_v[l](job->video_data[c]->data,
				job->video_data[c]->stride,
				job->video_data[c]->width,
				job->video_data[c]->height);

#ifdef DEBUG_P_BLOCK
      if (job->number == DEBUG_P_JOB && c == DEBUG_P_COMP) {
        printf("-----------------------------------------------------------------\n");
        printf("Transform V%d\n", l);
        printf("-----------------------------------------------------------------\n");
        __debug_print_slice(job, mSampleSize);
        printf("-----------------------------------------------------------------\n");
      }
#endif

			transforms_final(job->video_data[c]->data,
				job->video_data[c]->stride,
				job->odata[c],
				job->ostride[c],
				job->video_data[c]->width,
				job->video_data[c]->height,
				job->output_x[c],
				job->output_y[c],
				job->output_w[c],
				job->output_h[c]);

#ifdef DEBUG_P_BLOCK
			if (job->number == DEBUG_P_JOB && c == DEBUG_P_COMP) {
				printf("-----------------------------------------------------------------\n");
				printf("Transform H%d\n", l);
				printf("-----------------------------------------------------------------\n");
				for (int y = DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H; y < DEBUG_P_SLICE_Y*DEBUG_P_SLICE_H + DEBUG_P_SLICE_H; y++) {
					int16_t *D = (int16_t *)&job->odata[DEBUG_P_COMP][2 * (y*job->ostride[DEBUG_P_COMP] + DEBUG_P_SLICE_X*DEBUG_P_SLICE_W - job->output_x[DEBUG_P_COMP])];
					printf("  ");
					for (int x = 0; x < DEBUG_P_SLICE_W; x++)
						printf("%+6d ", D[x]);
					printf("\n");
				}
				printf("-----------------------------------------------------------------\n");
			}
#endif
		}
	}
	if (mParams.colourise) {
		if (mParams.colourise_quantiser) {
			int min_q, max_q;
			min_q = 255;
			max_q = 0;
			for (int N = 0; N < job->slices_y*job->slices_x; N++) {
				if (job->coded_slices[N].qindex > max_q)
					max_q = job->coded_slices[N].qindex;
				if (job->coded_slices[N].qindex < min_q)
					min_q = job->coded_slices[N].qindex;
			}

#ifdef DEBUG
			writelog(LOG_INFO, "Quantisers: [%d, %d]\n", min_q, max_q);
#endif

			int q_mult_fact = ((1 << 10) - 1) / (max_q - min_q);

			for (int c = 1; c < 3; c++) {
				int slice_width = (mWidth + mParams.transform_params.slices_x - 1) / mSlicesX / 2;
				int slice_height = (mHeight + mParams.transform_params.slices_y - 1) / mSlicesY;
				for (int Y = 0; Y < job->slices_y; Y++) {
					for (int X = 0; X < job->slices_x; X++) {
						CodedSlice *slice = &job->coded_slices[Y*job->slices_x + X];
						for (int y = 0; y < slice_height; y++) {
							for (int x = 0; x < slice_width; x++) {
								int yy = (Y*slice_height + y) - job->output_y[c];
								int xx = (X*slice_width + x) - job->output_x[c];
								if (yy >= 0 && yy < job->output_h[c] && xx >= 0 && xx < job->output_w[c])
									((uint16_t *)job->odata[c])[yy*job->ostride[c] + xx] = (slice->qindex - min_q)*q_mult_fact;
							}
						}
					}
				}
			}
		}
		else if (mParams.colourise_padding) {
			int min_p, max_p;
			min_p = 255 * mParams.slice_size_scalar;
			max_p = 0;
			for (int N = 0; N < job->slices_y*job->slices_x; N++) {
				if (job->coded_slices[N].padding > max_p)
					max_p = job->coded_slices[N].padding;
				if (job->coded_slices[N].padding < min_p)
					min_p = job->coded_slices[N].padding;
			}

#ifdef DEBUG
			writelog(LOG_INFO, "Padding: [%d, %d]\n", min_p, max_p);
#endif

			int p_mult_fact = ((1 << 10) - 1) / (max_p - min_p);

			for (int c = 1; c < 3; c++) {
				int slice_width = (mWidth + mParams.transform_params.slices_x - 1) / mSlicesX / 2;
				int slice_height = (mHeight + mParams.transform_params.slices_y - 1) / mSlicesY;
				for (int Y = 0; Y < job->slices_y; Y++) {
					for (int X = 0; X < job->slices_x; X++) {
						CodedSlice *slice = &job->coded_slices[Y*job->slices_x + X];
						for (int y = 0; y < slice_height; y++) {
							for (int x = 0; x < slice_width; x++) {
								int yy = (Y*slice_height + y) - job->output_y[c];
								int xx = (X*slice_width + x) - job->output_x[c];
								if (yy >= 0 && yy < job->output_h[c] && xx >= 0 && xx < job->output_w[c])
									((uint16_t *)job->odata[c])[yy*job->ostride[c] + xx] = (slice->padding - min_p)*p_mult_fact;
							}
						}
					}
				}
			}
		}
		else if (mParams.colourise_unpadded) {
			for (int c = 1; c < 3; c++) {
				int slice_width = (mWidth + mParams.transform_params.slices_x - 1) / mSlicesX / 2;
				int slice_height = (mHeight + mParams.transform_params.slices_y - 1) / mSlicesY;
				for (int Y = 0; Y < job->slices_y; Y++) {
					for (int X = 0; X < job->slices_x; X++) {
						CodedSlice *slice = &job->coded_slices[Y*job->slices_x + X];
						for (int y = 0; y < slice_height; y++) {
							for (int x = 0; x < slice_width; x++) {
								int yy = (Y*slice_height + y) - job->output_y[c];
								int xx = (X*slice_width + x) - job->output_x[c];
								if (yy >= 0 && yy < job->output_h[c] && xx >= 0 && xx < job->output_w[c]) {
									if (slice->padding == 0) {
										((uint16_t *)job->odata[c])[yy*job->ostride[c] + xx] = (1 << 10) - 1;
									}
									else {
										((uint16_t *)job->odata[c])[yy*job->ostride[c] + xx] = (1 << 9);
									}
								}
							}
						}
					}
				}
			}
		}
		else {
			for (int c = 1; c < 3; c++) {
				for (int y = 0; y < job->output_h[c]; y++)
					for (int x = 0; x < job->output_w[c]; x++)
						((uint16_t *)job->odata[c])[y*job->ostride[c] + x] = (1 << 9);
			}
		}
	}
}
