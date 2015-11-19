/*****************************************************************************
 * vc2hqdecodestrings.hpp : human readable strings for VC2 debugging
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



#ifndef __VC2HQDECODESTRINGS_H__
#define __VC2HQDECODESTRINGS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *VC2DecoderBaseVideoFormatString[] = { "CUSTOM",
                                                  "QSIF 525",
                                                  "QCIF",
                                                  "SIF 525",
                                                  "CIF",
                                                  "4SIF 525",
                                                  "4CIF",
                                                  "SD 480i60",
                                                  "SD 576i50",
                                                  "HD 720p60",
                                                  "HD 720p50",
                                                  "HD 1080i60",
                                                  "HD 1080i50",
                                                  "HD 1080p60",
                                                  "HD 1080p50",
                                                  "DC 2K",
                                                  "DC 4K",
                                                  "UHD1 60Hz",
                                                  "UHD1 50Hz",
                                                  "UHD2 60Hz",
                                                  "UHD2 50Hz",
                                                  "HD 1080p24",
                                                  "SDPRO486" };

const char *VC2DecoderSubsamplingFormatString[] = { "4:2:0",
                                                    "4:2:2",
                                                    "4:4:4" };

const char *VC2DecoderPictureCodingModeString[] = { "Progressive",
                             "Interlaced" };

const char *VC2DecoderFrameRateString[] = { "Custom",
                                            "23.98",
                                            "24",
                                            "25",
                                            "29.97",
                                            "true 30",
                                            "50"
                                            "59.94",
                                            "true 60",
                                            "14.99",
                                            "12.5",
                                            "48" };

const char *VC2DecoderPixelAspectString[] = { "Custom",
                                              "1:1",
                                              "10:11",
                                              "12:11",
                                              "40:33",
                                              "16:11",
                                              "4:3" };

const char *VC2DecoderPresetSignalRangeStringLuma[] = { "Custom",
                                                        "0 - 255",
                                                        "16 - 235",
                                                        "64 - 940",
                                                        "256 - 3760" };

const char *VC2DecoderPresetSignalRangeStringColorDiff[] = { "Custom",
                                                             "0 - 256",
                                                             "16 - 240",
                                                             "64 - 960",
                                                             "256 - 3840" };

const int VC2DecoderColorSpecIndices[][3] = { { 0, 0, 0},
                                              { 1, 1, 0},
                                              { 2, 1, 0},
                                              { 0, 0, 0},
                                              { 3, 2, 3} };

const char *VC2DecoderColorPrimariesString[] = { "HDTV (ITU-R BT.709)",
                                                 "SDTV 525 (ITU-R BT.601)",
                                                 "SDTV 625 (ITU-R BT.601)",
                                                 "D-Cinema (SMPTE 428.1)" };

const char *VC2DecoderColorMatrixString[] = { "HDTV (ITU-R BT.709)",
                                              "SDTV (ITU-R BT.601)",
                                              "Reversible (ITU-T H.264)",
                                              "RGB" };

const char *VC2DecoderTransferFunctionString[] = { "TV (ITU-R BT.709)",
                                                   "Extended Gamut (ITU-R BT.1361)",
                                                   "Linear",
                                                   "D-Cinema (SMPTE 428.1)" };

const char *VC2DecoderWaveletFilterTypeString[] = { "Deslauriers-Dubuc 9,7",
                                                    "LeGall 5,3",
                                                    "Deslauriers-Dubuc 13,7",
                                                    "Haar (0 shift)",
                                                    "Haar (1 shift)",
                                                    "Fidelity",
                                                    "Daubechies 9,7" };

const char *VC2DecoderErrorString[] = {
  "VC2DECODER_OK",
  "VC2DECODER_BADPARAMS",
  "VC2DECODER_DECODE_FAILED",
  "VC2DECODER_BADTHREAD",
  "VC2DECODER_NOTIMPLEMENTED",
  "VC2DECODER_NOQUANTISER",
  "VC2DECODER_CODEROVERRUN",
  "VC2DECODER_NOTPARSEINFO",
  "VC2DECODER_BADSTREAM"
};

#ifdef __cplusplus
};
#endif

#endif /* __VC2HQDECODE_H__ */
