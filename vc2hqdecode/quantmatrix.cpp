/*****************************************************************************
 * quantmatrix.cpp : Quantization Matrices
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

#include "logger.hpp"
#include "internal.h"
#include "dequantise.hpp"

#include "platform_variant.hpp"

int32_t quant_factor(int i) {
	int b = 1 << (i / 4);
	if (i % 4 == 0)
		return 4 * b;
	else if (i % 4 == 1)
		return (((503829 * b) + 52958) / 105917);
	else if (i % 4 == 2)
		return (((665857 * b) + 58854) / 117708);
	else
		return (((440253 * b) + 32722) / 65444);
}

int32_t quant_offset(int i) {
	if (i == 0)
		return 1;
	else if (i == 1)
		return 2;
	else
		return (quant_factor(i) + 1) / 2;
}

const int DEFAULT_QUANTISATION_MATRIX_ADJUSTMENTS[VC2DECODER_WFT_NUM][5][5][4] = {
	// VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		}
	},
	// VC2DECODER_WFT_LEGALL_5_3
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 4, 0, 0, 0 },
			{ 0, 2, 2, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 5, 5, 3 },
			{ 0, 7, 7, 5 }
		},
		{
			{ 4, 0, 0, 0 },
			{ 0, 2, 2, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 5, 5, 3 },
			{ 0, 7, 7, 5 }
		},
		{
			{ 4, 0, 0, 0 },
			{ 0, 2, 2, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 5, 5, 3 },
			{ 0, 7, 7, 5 }
		},
		{
			{ 4, 0, 0, 0 },
			{ 0, 2, 2, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 5, 5, 3 },
			{ 0, 7, 7, 5 }
		}
	},
	// VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		},
		{
			{ 5, 0, 0, 0 },
			{ 0, 3, 3, 0 },
			{ 0, 4, 4, 1 },
			{ 0, 5, 5, 2 },
			{ 0, 6, 6, 3 }
		}
	},
	// VC2DECODER_WFT_HAAR_NO_SHIFT
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 8, 0, 0, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 12, 0, 0, 0 },
			{ 0, 8, 8, 4 },
			{ 0, 4, 4, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 16, 0, 0, 0 },
			{ 0, 12, 12, 8 },
			{ 0, 8, 8, 4 },
			{ 0, 4, 4, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 20, 0, 0, 0 },
			{ 0, 16, 16, 12 },
			{ 0, 12, 12, 8 },
			{ 0, 8, 8, 4 },
			{ 0, 4, 4, 0 }
		}
	},
	// VC2DECODER_WFT_HAAR_SINGLE_SHIFT
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 8, 0, 0, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 }
		},
		{
			{ 8, 0, 0, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 }
		},
		{
			{ 8, 0, 0, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 }
		},
		{
			{ 8, 0, 0, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 },
			{ 0, 4, 4, 0 }
		}
	},
	// VC2DECODER_WFT_FIDELITY
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 4, 4, 8 },
			{ 0, 8, 8, 12 },
			{ 0, 13, 13, 17 },
			{ 0, 17, 17, 21 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 4, 4, 8 },
			{ 0, 8, 8, 12 },
			{ 0, 13, 13, 17 },
			{ 0, 17, 17, 21 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 4, 4, 8 },
			{ 0, 8, 8, 12 },
			{ 0, 13, 13, 17 },
			{ 0, 17, 17, 21 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 4, 4, 8 },
			{ 0, 8, 8, 12 },
			{ 0, 13, 13, 17 },
			{ 0, 17, 17, 21 }
		}
	},
	// VC2DECODER_WFT_DAUBECHIES_9_7
	{
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 3, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 6, 6, 5 },
			{ 0, 9, 9, 7 }
		},
		{
			{ 3, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 6, 6, 5 },
			{ 0, 9, 9, 7 }
		},
		{
			{ 3, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 6, 6, 5 },
			{ 0, 9, 9, 7 }
		},
		{
			{ 3, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 4, 4, 2 },
			{ 0, 6, 6, 5 },
			{ 0, 9, 9, 7 }
		}
	}
};


VC2HQDECODE_API QuantisationMatrix *quantisation_matrices(uint32_t wavelet_index, int depth, int qindex_max) {
	if (depth > 4) {
		writelog(LOG_ERROR, "%s:%d:  Could not form quantisation matrices, depth greater than 4 not supported\n", __FILE__, __LINE__);
		throw VC2DECODER_NOTIMPLEMENTED;
	}

	QuantisationMatrix *matrix = new QuantisationMatrix[qindex_max + 1];

	__m128i **pointers_qf_1 = new __m128i*[(depth + 1)*(qindex_max + 1)];
	__m128i **pointers_qo_1 = new __m128i*[(depth + 1)*(qindex_max + 1)];
	__m128i *pointers_qf_2 = (__m128i *)ALIGNED_ALLOC(16, 4 * (depth + 1)*(qindex_max + 1)*sizeof(__m128i)); //new __m128i[4*(depth + 1)*(qindex_max + 1)];
	__m128i *pointers_qo_2 = (__m128i *)ALIGNED_ALLOC(16, 4 * (depth + 1)*(qindex_max + 1)*sizeof(__m128i));

	for (int q = 0; q <= qindex_max; q++) {
		matrix[q].qfactor = &pointers_qf_1[(depth + 1)*q];
		matrix[q].qoffset = &pointers_qo_1[(depth + 1)*q];

		for (int l = 0; l <= depth; l++) {
			matrix[q].qfactor[l] = &pointers_qf_2[4 * ((depth + 1)*q + l)];
			matrix[q].qoffset[l] = &pointers_qo_2[4 * ((depth + 1)*q + l)];

			for (int s = 0; s < 4; s++) {
				int qi = (q > DEFAULT_QUANTISATION_MATRIX_ADJUSTMENTS[wavelet_index][depth][l][s]) ? (q - DEFAULT_QUANTISATION_MATRIX_ADJUSTMENTS[wavelet_index][depth][l][s]) : 0;
				matrix[q].qfactor[l][s] = _mm_set1_epi32(quant_factor(qi));
				matrix[q].qoffset[l][s] = _mm_set1_epi32(quant_offset(qi) + 2);
			}
		}
	}

	return matrix;
}

void delete_matrices(QuantisationMatrix *matrix) {
	ALIGNED_FREE(matrix[0].qfactor[0]);
	ALIGNED_FREE(matrix[0].qoffset[0]);
	delete[] matrix[0].qfactor;
	delete[] matrix[0].qoffset;
	delete[] matrix;
}
