/*****************************************************************************
 * VideoFormat.hpp : predefined video formats
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

#ifndef __VIDEOFORMAT_HPP__
#define __VIDEOFORMAT_HPP__

#include "internal.h"

namespace vc2 {
	struct FrameRate {
		uint32_t numer;
		uint32_t denom;
	};

	const FrameRate preset_framerates[] = {
		{
			0, /* numer */
			0 /* denom */
		},
		{
			24000, /* numer */
			1001 /* denom */
		},
		{
			24, /* numer */
			1 /* denom */
		},
		{
			25, /* numer */
			1 /* denom */
		},
		{
			30000, /* numer */
			1001 /* denom */
		},
		{
			30, /* numer */
			1 /* denom */
		},
		{
			50, /* numer */
			1 /* denom */
		},
		{
			60000, /* numer */
			1001 /* denom */
		},
		{
			60, /* numer */
			1 /* denom */
		},
		{
			15000, /* numer */
			1001 /* denom */
		},
		{
			25, /* numer */
			2 /* denom */
		},
		{
			48, /* numer */
			1 /* denom */
		}
	};

	const uint32_t preset_pixel_aspect_ratios[][2] = {
		{ 0, 0 },
		{ 1, 1 },
		{ 10, 11 },
		{ 12, 11 },
		{ 40, 33 },
		{ 16, 11 },
		{ 4, 3 }
	};

	struct SignalRange {
		uint32_t luma_offset;
		uint32_t luma_excursion;
		uint32_t luma_bytes_per_sample;
		uint32_t luma_active_bits;
		uint32_t color_diff_offset;
		uint32_t color_diff_excursion;
		uint32_t color_diff_bytes_per_sample;
		uint32_t color_diff_active_bits;
	};

	const SignalRange preset_signal_ranges[] = {
		// VC2DECODER_PSR_CUSTOM
		{
			0, /* luma_offset */
			0, /* luma_excursion */
		0, /* luma_bytes_per_sample */
		0, /* luma_active_bits */
		0, /* color_diff_offset */
		0, /* color_diff_excursion */
		0, /* color_diff_bytes_per_sample */
		0 /* color_diff_active_bits */
		},
		// VC2DECODER_PSR_8BITFULL
		{
			0, /* luma_offset */
			255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8 /* color_diff_active_bits */
		},
		// VC2DECODER_PSR_8BITVID
		{
			16, /* luma_offset */
			219, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		224, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8 /* color_diff_active_bits */
		},
		// VC2DECODER_PSR_10BITVID
		{
			64, /* luma_offset */
			876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10 /* color_diff_active_bits */
		},
		// VC2DECODER_PSR_12BITVID
		{
			256, /* luma_offset */
			3504, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		12, /* luma_active_bits */
		2048, /* color_diff_offset */
		3584, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		12 /* color_diff_active_bits */
		}
	};

	struct ColorSpec {
		uint32_t color_primaries;
		uint32_t color_matrix;
		uint32_t transfer_function;
	};

	const ColorSpec preset_color_specs[] = {
		//   VC2DECODER_CSP_CUSTOM
		{
			0, /* color_primaries */
			0, /* color_matrix */
		0 /* transfer_function */
		},
		//  VC2DECODER_CSP_SDTV525
		{
			VC2DECODER_CPR_SDTV525, /* color_primaries */
			VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		//  VC2DECODER_CSP_SDTV625
		{
			VC2DECODER_CPR_SDTV625, /* color_primaries */
			VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		//  VC2DECODER_CSP_HDTV
		{
			VC2DECODER_CPR_HDTV, /* color_primaries */
			VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		//  VC2DECODER_CSP_DCINE
		{
			VC2DECODER_CPR_DCINE, /* color_primaries */
			VC2DECODER_CMA_REVERSIBLE, /* color_matrix */
		VC2DECODER_TRF_DCINE /* transfer_function */
		}
	};

	struct VideoFormat {
		uint32_t frame_width;
		uint32_t frame_height;
		uint32_t color_diff_format_index;
		uint32_t source_sampling;
		bool     top_field_first;
		uint32_t frame_rate_numer;
		uint32_t frame_rate_denom;
		uint32_t pixel_aspect_ratio_numer;
		uint32_t pixel_aspect_ratio_denom;
		uint32_t clean_width;
		uint32_t clean_height;
		uint32_t left_offset;
		uint32_t top_offset;
		uint32_t luma_offset;
		uint32_t luma_excursion;
		uint32_t luma_bytes_per_sample;
		uint32_t luma_active_bits;
		uint32_t color_diff_offset;
		uint32_t color_diff_excursion;
		uint32_t color_diff_bytes_per_sample;
		uint32_t color_diff_active_bits;
		uint32_t color_primaries;
		uint32_t color_matrix;
		uint32_t transfer_function;
	};

	const VideoFormat preset_formats[] = {
		// VC2DECODER_BVF_CUSTOM
		{
			640, /* frame_width */
			480, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		false, /* top_field_first */
		24000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		640, /* clean_width */
		480, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_QSIF525
		{
			176, /* frame_width */
			120, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		false, /* top_field_first */
		15000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		10, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		176, /* clean_width */
		120, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV525, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_QCIF
		{
			176, /* frame_width */
			144, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		25, /* frame_rate_numer */
		2, /* frame_rate_denom */
		12, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		176, /* clean_width */
		144, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV625, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_SIF525
		{
			352, /* frame_width */
			240, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		false, /* top_field_first */
		15000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		10, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		352, /* clean_width */
		240, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV525, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_CIF
		{
			352, /* frame_width */
			288, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		25, /* frame_rate_numer */
		2, /* frame_rate_denom */
		12, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		352, /* clean_width */
		288, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV625, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_4SIF525
		{
			704, /* frame_width */
			480, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		false, /* top_field_first */
		15000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		10, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		704, /* clean_width */
		480, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV525, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_4CIF
		{
			704, /* frame_width */
			576, /* frame_height */
		VC2DECODER_CDS_420, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		25, /* frame_rate_numer */
		2, /* frame_rate_denom */
		12, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		704, /* clean_width */
		576, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		0, /* luma_offset */
		255, /* luma_excursion */
		1, /* luma_bytes_per_sample */
		8, /* luma_active_bits */
		128, /* color_diff_offset */
		255, /* color_diff_excursion */
		1, /* color_diff_bytes_per_sample */
		8, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV625, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_SD480I_60
		{
			720, /* frame_width */
			480, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		1, /* source_sampling */
		false, /* top_field_first */
		30000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		10, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		704, /* clean_width */
		480, /* clean_height */
		8, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV525, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_SD576I_50
		{
			720, /* frame_width */
			576, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		1, /* source_sampling */
		true, /* top_field_first */
		25, /* frame_rate_numer */
		1, /* frame_rate_denom */
		12, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		704, /* clean_width */
		576, /* clean_height */
		8, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_SDTV625, /* color_primaries */
		VC2DECODER_CMA_SDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD720P_60
		{
			1280, /* frame_width */
			720, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		60000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1280, /* clean_width */
		720, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_HD720P_50
		{
			1280, /* frame_width */
			720, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		50, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1280, /* clean_width */
		720, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD1080I_60
		{
			1920, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		1, /* source_sampling */
		true, /* top_field_first */
		30000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1920, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD1080I_50
		{
			1920, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		1, /* source_sampling */
		true, /* top_field_first */
		25, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1920, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD1080P_60
		{
			1920, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		60000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1920, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD1080P_50
		{
			1920, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		50, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1920, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_DC2K
		{
			2048, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_444, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		24, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		2048, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		256, /* luma_offset */
		3504, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		12, /* luma_active_bits */
		2048, /* color_diff_offset */
		3584, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		12, /* color_diff_active_bits */
		VC2DECODER_CPR_DCINE, /* color_primaries */
		VC2DECODER_CMA_REVERSIBLE, /* color_matrix */
		VC2DECODER_TRF_DCINE /* transfer_function */
		},
		// VC2DECODER_BVF_DC4K
		{
			4096, /* frame_width */
			2160, /* frame_height */
		VC2DECODER_CDS_444, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		24, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		4096, /* clean_width */
		2160, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		256, /* luma_offset */
		3504, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		12, /* luma_active_bits */
		2048, /* color_diff_offset */
		3584, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		12, /* color_diff_active_bits */
		VC2DECODER_CPR_DCINE, /* color_primaries */
		VC2DECODER_CMA_REVERSIBLE, /* color_matrix */
		VC2DECODER_TRF_DCINE /* transfer_function */
		},
		// VC2DECODER_BVF_UHDTV4K_60
		{
			3840, /* frame_width */
			2160, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		60000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		3840, /* clean_width */
		2160, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_UHDTV4K_50
		{
			3840, /* frame_width */
			2160, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		50, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		3840, /* clean_width */
		2160, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_UHDTV8K_60
		{
			7680, /* frame_width */
			4320, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		60000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		7680, /* clean_width */
		4320, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_UHDTV8K_50
		{
			7680, /* frame_width */
			4320, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		50, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		7680, /* clean_width */
		4320, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_HD1080P_24
		{
			1920, /* frame_width */
			1080, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		0, /* source_sampling */
		true, /* top_field_first */
		24, /* frame_rate_numer */
		1, /* frame_rate_denom */
		1, /* pixel_aspect_ratio_numer */
		1, /* pixel_aspect_ratio_denom */
		1920, /* clean_width */
		1080, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		},
		// VC2DECODER_BVF_SDPRO486
		{
			720, /* frame_width */
			486, /* frame_height */
		VC2DECODER_CDS_422, /* color_diff_format_index */
		1, /* source_sampling */
		false, /* top_field_first */
		30000, /* frame_rate_numer */
		1001, /* frame_rate_denom */
		10, /* pixel_aspect_ratio_numer */
		11, /* pixel_aspect_ratio_denom */
		720, /* clean_width */
		486, /* clean_height */
		0, /* left_offset */
		0, /* top_offset */
		64, /* luma_offset */
		876, /* luma_excursion */
		2, /* luma_bytes_per_sample */
		10, /* luma_active_bits */
		512, /* color_diff_offset */
		896, /* color_diff_excursion */
		2, /* color_diff_bytes_per_sample */
		10, /* color_diff_active_bits */
		VC2DECODER_CPR_HDTV, /* color_primaries */
		VC2DECODER_CMA_HDTV, /* color_matrix */
		VC2DECODER_TRF_TVGAMMA /* transfer_function */
		}
	};

	const uint32_t transform_block_overlap[] = {
		//  VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7
		0,
		//  VC2DECODER_WFT_LEGALL_5_3
		2,
		//  VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7
		0,
		//  VC2DECODER_WFT_HAAR_NO_SHIFT
		0,
		//  VC2DECODER_WFT_HAAR_SINGLE_SHIFT
		0,
		//  VC2DECODER_WFT_FIDELITY
		0,
		//  VC2DECODER_WFT_DAUBECHIES_9_7
		0
	};
}



#endif /* __VIDEOFORMAT_HPP__ */

