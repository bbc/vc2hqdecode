/*****************************************************************************
 * vc2hqdecode: test program
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

#include <string.h>
#include <vc2hqdecode/vc2hqdecode.h>
#include <vc2hqdecode/vc2hqdecodestrings.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <string>
#include <list>

#ifdef _WIN32
#include <Windows.h>

int64_t gettime(void)
{
  FILETIME wintime;
  GetSystemTimeAsFileTime(&wintime);

  return ((int64_t)wintime.dwHighDateTime << 32 | wintime.dwLowDateTime) / 10 - 11644473600000000;
}

inline FILE *FOPEN(const char *fname, const char *mode) {
  FILE *f;
  errno_t err = fopen_s(&f, fname, mode);
  if (err) {
    errno = err;
    return NULL;
  }

  return f;
}

#else
#define FOPEN fopen
#include <sys/time.h>

int64_t gettime(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif

#include "tclap/CmdLine.h"

void usage() {
  printf("Usage: vc2decode [options] input_file [output_file]\n");
  printf("  By default the output file will be the input file with .yuv appended\n");
}

void print_sequence_info(VC2DecoderSequenceInfo &info, bool verbose);

int main (int argc, char *argv[]) {
  /* Program Option parsing */

  int num_frames = 1;
  int threads = 1;
  bool disable_output = false;
  bool colourise_quantiser = false;
  bool colourise_padding = false;
  bool colourise_unpadded = false;
  bool quartersize = false;
  bool verbose = false;

  std::string input_filename;
  std::string output_filename;

  try {
    TCLAP::CmdLine cmd("VC2 HQ profile Decoder Example\n"
                       "All input files must be vc2 streams\n"
                       "All output files will be yuv422p10le\n", '=', "0.1", true);

    TCLAP::SwitchArg     verbose_arg             ("v", "verbose",        "verbose mode",                                    cmd, false);
    TCLAP::ValueArg<int> num_frames_arg          ("n", "num-frames",     "Number of frames to decode", false, 1, "integer", cmd);
    TCLAP::ValueArg<int> num_threads_arg         ("t", "threads",        "Number of threads",          false, 1, "integer", cmd);
    TCLAP::SwitchArg     disable_output_args     ("d", "disable-output",      "disable output",                                  cmd, false);
    TCLAP::SwitchArg     colourise_quantiser_args("q", "colourise-quantiser", "colourise based on quantiser levels",             cmd, false);
    TCLAP::SwitchArg     colourise_padding_args  ("p", "colourise-padding", "colourise based on padding levels",               cmd, false);
    TCLAP::SwitchArg     colourise_unpadded_args ("u", "colourise-unpadded", "colourise based on lack of padding",              cmd, false);
    
    TCLAP::UnlabeledValueArg<std::string> input_file_arg("input_file",   "encoded input file",         true, "", "string",  cmd);
    TCLAP::UnlabeledValueArg<std::string> output_file_arg("output_file", "output file (defaults to input file + .yuv)", false, "", "string", cmd);

    cmd.parse( argc, argv );

    num_frames          = num_frames_arg.getValue();
    threads             = num_threads_arg.getValue();
    disable_output      = disable_output_args.getValue();
    colourise_quantiser = colourise_quantiser_args.getValue();
    colourise_padding   = colourise_padding_args.getValue();
    colourise_unpadded  = colourise_unpadded_args.getValue();
    verbose             = verbose_arg.getValue();

    input_filename = input_file_arg.getValue();
    output_filename = output_file_arg.getValue();
  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; return 1;
  }

  if (output_filename == "")
    output_filename = input_filename + ".yuv";

  /* Variables for storing input and output buffers */
  char *idata;
  size_t input_length = 0;
  uint16_t **odata = new uint16_t*[num_frames];   // Output frame pointers
  uint16_t ***opics = new uint16_t**[num_frames*2]; // Output picture pointers (which may be different for interlaced sequences)
  for (int i = 0; i < num_frames * 2; i++) {
    opics[i] = new uint16_t*[3];
  }
  int ostride[3];
  int num_frames_decoded_from_sequence = 0;
  int total_frames_decoded = 0;




  
  /* Initialise decoder */
  vc2decode_init();
  VC2DecoderHandle decoder = vc2decode_create();

  /* Configure decoder */
  {
    VC2DecoderParamsUser params;
    memset((void *)&params, 0, sizeof(params));

    params.threads = threads;
    params.colourise_quantiser = colourise_quantiser;
    params.colourise_padding   = colourise_padding;
    params.colourise_unpadded  = colourise_unpadded;


    /* QuarterSize is only really sensible for HD */
    if (quartersize) {
      params.partial_decode = true;
      params.partial_decode_width  = 1920/2;
      params.partial_decode_height = 1080/2;

      params.partial_decode_offset_x  = 0;
      params.partial_decode_offset_y  = 540;
    }


    {
      VC2DecoderResult r = vc2decode_set_parameters(decoder, params);
      if (r != VC2DECODER_OK) {
        printf("Parameter Error: %d\n", r);
        return 1;
      }
    }
  }


  /* Read input data */
  {
    FILE *f = FOPEN(input_filename.c_str(), "rb");
    if (!f) {
      fprintf(stderr, "Could not open: %s\n", input_filename.c_str());
      perror("Invalid input file: ");
      return 1;
    }
    int r = fseek(f, 0L, SEEK_END);
    if (r < 0) {
      perror("Error seeking in input file");
      return 1;
    }
    input_length = ftell(f);
    rewind(f);

    idata = (char *)malloc(input_length);
    size_t s = fread(idata, 1, input_length, f);
    if (s != input_length) {
      if (ferror(f))
        fprintf(stderr, "Error reading file\n");
      else
        fprintf(stderr, "Improper Length on Input File\n");
      return 1;
    }

    fclose(f);

    printf("Read %lubytes of input data\n", input_length);
  }


  /* Set the output fmt to something invalid to start with */
  VC2DecoderOutputFormat fmt;
  fmt.width            = 0;
  fmt.height           = 0;
  fmt.signal_range     = 0;
  fmt.source_sampling  = 0;
  fmt.frame_rate_numer = 0;
  fmt.frame_rate_denom = 0;
  fmt.interlaced       = 0;

  ostride[0] = 0;
  ostride[1] = 0;
  ostride[2] = 0;

  for (int i = 0; i < num_frames; i++) {
    odata[i] = NULL;
  }




  /* Begin the main program loop */
  int err = 0;
  uint64_t time_taken = 0;
  while (total_frames_decoded < num_frames) {
    VC2DecoderResult r;

    /* Reset to the start of the input data */
    char *id = idata;
    char *iend = idata + input_length;

    /* We are at the start of a sequence, so synchronise */
    r = vc2decode_synchronise(decoder, &id, iend - id, true);
    if (r != VC2DECODER_OK_RECONFIGURED) {
      fprintf(stderr, "Error synchronising to sequence");
      err = 1;
      break;
    }

    /* We have synchronised, so get the output format */
    VC2DecoderOutputFormat new_fmt;
    r = vc2decode_get_output_format(decoder, &new_fmt);
    if (r != VC2DECODER_OK) {
      fprintf(stderr, "Output format error: %d\n", r);
      err = 1;
      break;
    }

    /* Reconfigure output buffers if this format doesn't
       match the current one */
    if (fmt.width != new_fmt.width ||
        fmt.height != new_fmt.height ||
        fmt.interlaced != new_fmt.interlaced) {
      fmt = new_fmt;

      for (int i = 0; i < num_frames; i++) {
        if (odata[i])
          free(odata[i]);
        odata[i] = (uint16_t *)malloc(fmt.width*fmt.height*2*sizeof(uint16_t));
      }

      if (fmt.interlaced) {
        ostride[0] = fmt.width*2;
        ostride[1] = fmt.width;
        ostride[2] = fmt.width;
        for (int i = 0; i < num_frames; i++) {
          opics[2*i + 0][0] = odata[i];
          opics[2*i + 0][1] = odata[i] + fmt.width*fmt.height;
          opics[2*i + 0][2] = odata[i] + fmt.width*fmt.height + fmt.width*fmt.height/2;

          opics[2*i + 1][0] = odata[i] + fmt.width;
          opics[2*i + 1][1] = odata[i] + fmt.width*fmt.height + fmt.width/2;
          opics[2*i + 1][2] = odata[i] + fmt.width*fmt.height + fmt.width*fmt.height/2 + fmt.width/2;
        }
      } else {
        ostride[0] = fmt.width;
        ostride[1] = fmt.width/2;
        ostride[2] = fmt.width/2;
        for (int i = 0; i < num_frames; i++) {
          opics[i][0] = odata[i];
          opics[i][1] = odata[i] + fmt.width*fmt.height;
          opics[i][2] = odata[i] + fmt.width*fmt.height + fmt.width*fmt.height/2;
        }
      }
    }

    /* Decode Pictures from stream */
    int picture = 0;
    int64_t start, end;
    start = gettime();
    while (total_frames_decoded < num_frames) {
      r = vc2decode_decode_one_picture(decoder, &id, iend - id, opics[picture], ostride, true);

      /* If End of Sequence break from loop */
      if (r == VC2DECODER_OK_EOS) {
        break;
      }

      /* If a picture has been sucessfully decoded continue */
      if (r == VC2DECODER_OK_PICTURE) {
        if (!fmt.interlaced || (picture%2))
          total_frames_decoded++;
        picture++;
        if (id == NULL) {
          fprintf(stderr, "Premature end of stream\n");
          break;
        }
        continue;
      }

      /* If a reconfiguration has been triggered this is an invalid sequence, flag that */
      if (r == VC2DECODER_OK_RECONFIGURED) {
        fprintf(stderr, "Warning: this sequence changes parameters part way through\n");
        break;
      }

      /* Otherwise we have an error */
      if (r < 0) {
        fprintf(stderr, "Error decoding:\n");
        fprintf(stderr, "  %s\n", VC2DecoderErrorString[-r]);
        err=1;
        break;
      }

      fprintf(stderr, "Unknown Return value: %d\n", r);
      break;
    }
    end = gettime();
    /* We have reached the end of a sequence, or have decoded enough frames */

    /*If an error has occurred bail */
    if (err)
      break;

    /*If no pictures were decoded bail */
    if (picture == 0) {
      fprintf(stderr, "No pictures in sequence!\n");
      err = 1;
      break;
    }

    /* Otherwise update the record of how many decoded frames have been recorded */
    int num_frames_decoded_from_this_sequence = (fmt.interlaced)?(picture/2):(picture);
    if (num_frames_decoded_from_this_sequence > num_frames_decoded_from_sequence)
      num_frames_decoded_from_sequence = num_frames_decoded_from_this_sequence;

    /* And update the record of time taken to decode */
    time_taken += end - start;
  }

  /* If there has been no error print the speed */
  if (!err) {
    printf("--------------------------------------------------\n");
    printf("  %d frames decoded in %5.3fs\n", total_frames_decoded, time_taken/1000000.0);
    printf("  %5.3ffps\n", total_frames_decoded*1000000.0/time_taken);
    printf("--------------------------------------------------\n");
  }

  /* If there has been no error get decoder statistics  */
  if (!err) {
    VC2DecoderSequenceInfo info;
    vc2decode_sequence_info(decoder, &info);

    print_sequence_info(info, verbose);
  }

  /*
     If there is no error and output is enabled write the output to a file

     This code actually performs an endianess swap and some bit shifting on the output
     data to put it into the desired format (10-bit data in the high order bits of 16-bit
     words in network byte order).
   */
  if (!err && !disable_output) {
    try {
      FILE *of = FOPEN(output_filename.c_str(), "wb");

      for (int i = 0; i < num_frames_decoded_from_sequence; i++) {

        for (int y = 0; y < fmt.height; y++) {
          size_t s = fwrite(&odata[i][(y*fmt.width)], 1, fmt.width*sizeof(uint16_t), of);
          if (s != fmt.width*sizeof(uint16_t))
            throw std::runtime_error("Writing Error");
        }
        for (int y = 0; y < fmt.height*2; y++) {
          size_t s = fwrite(&odata[i][(fmt.height*fmt.width + y*fmt.width/2)], 1, fmt.width/2*sizeof(uint16_t), of);
          if (s != fmt.width/2*sizeof(uint16_t))
            throw std::runtime_error("Writing Error");
        }
      }
      printf("Wrote out %d frames\n", num_frames_decoded_from_sequence);
      fclose(of);
    } catch(...) {
      fprintf(stderr, "Error writing output\n");
      err = 1;
    }
  }

  /* Destroy the decoder */
  vc2decode_destroy(decoder);


  /* Deallocate buffers */
  free(idata);

  for (int i = 0; i < num_frames; i++)
    if (odata[i])
      free(odata[i]);

  delete[] odata;
  for (int i = 0; i < num_frames * 2; i++)
    delete[] opics[i];
  delete[] opics;

  return err;
}


void print_sequence_info(VC2DecoderSequenceInfo &info, bool /*verbose*/) {
  printf("--------------------------------------------------------------------------------\n");
  printf("  Sequence Headers: %d\n", info.sequence_headers_seen);
  printf("\n");
  printf("    Video Format:\n");
  printf("      Base format                      : %s (%2d)\n", VC2DecoderBaseVideoFormatString[info.video_format.base_video_format], info.video_format.base_video_format);
  if (info.video_format.custom_dimensions_flag)
    printf("      + Dimension                      : %dx%d\n", info.video_format.frame_width, info.video_format.frame_height);
  if (info.video_format.custom_color_diff_format_flag)
    printf("      + Colour Diff                    : %s (%d)\n", VC2DecoderSubsamplingFormatString[info.video_format.color_diff_format_index], info.video_format.color_diff_format_index);
  if (info.video_format.custom_scan_format_flag)
    printf("      + Scan Format                    : %s (%d)\n", VC2DecoderPictureCodingModeString[info.video_format.source_sampling], info.video_format.source_sampling);
  if (info.video_format.custom_frame_rate_flag) {
    if (info.video_format.frame_rate_index == 0)
      printf("      + Frame Rate                     : %d/%d fps\n", info.video_format.frame_rate_numer, info.video_format.frame_rate_denom);
    else
      printf("      + Frame Rate                     : %s fps\n", VC2DecoderFrameRateString[info.video_format.frame_rate_index]);
  }
  if (info.video_format.custom_pixel_aspect_ratio_flag) {
    if (info.video_format.pixel_aspect_ratio_index == 0)
      printf("      + Pixel Aspect Ratio             : %d:%d\n", info.video_format.pixel_aspect_ratio_numer, info.video_format.pixel_aspect_ratio_denom);
    else
      printf("      + Pixel Aspect Ratio             : %s\n", VC2DecoderPixelAspectString[info.video_format.pixel_aspect_ratio_index]);
  }
  if (info.video_format.custom_clean_area_flag) {
    printf("      + Clean Area                     : (%d - %d)x(%d - %d)\n", info.video_format.left_offset, info.video_format.left_offset + info.video_format.clean_width,
           info.video_format.top_offset, info.video_format.top_offset + info.video_format.clean_height);
  }
  if (info.video_format.custom_signal_range_flag) {
    if (info.video_format.signal_range_index == 0) {
      printf("      + Signal Range (luma)            : %d - %d\n", info.video_format.luma_offset, info.video_format.luma_offset + info.video_format.luma_excursion);
      printf("      + Signal Range (chroma)          : %+d - %+d\n", info.video_format.color_diff_offset - (info.video_format.color_diff_excursion)/2, info.video_format.color_diff_offset + (info.video_format.color_diff_excursion + 1)/2);
    }
    else {
      printf("      + Signal Range (luma)            : %s\n", VC2DecoderPresetSignalRangeStringLuma[info.video_format.signal_range_index]);
      printf("      + Signal Range (chroma)          : %s\n", VC2DecoderPresetSignalRangeStringColorDiff[info.video_format.signal_range_index]);
    }
  }
  if (info.video_format.custom_color_spec_flag) {
    if (info.video_format.color_spec_index == 0) {
      if (info.video_format.custom_color_primaries_flag)
        printf("      + Color Primaries                : %s\n", VC2DecoderColorPrimariesString[info.video_format.color_primaries_index]);
      if (info.video_format.custom_color_matrix_flag)
        printf("      + Color Matrix                   : %s\n", VC2DecoderColorMatrixString[info.video_format.color_matrix_index]);
      if (info.video_format.custom_transfer_function_flag)
        printf("      + Transfer Function              : %s\n", VC2DecoderTransferFunctionString[info.video_format.transfer_function_index]);
    } else {
      printf("      + Color Primaries                : %s\n", VC2DecoderColorPrimariesString[VC2DecoderColorSpecIndices[info.video_format.color_spec_index][0]]);
      printf("      + Color Matrix                   : %s\n", VC2DecoderColorMatrixString[VC2DecoderColorSpecIndices[info.video_format.color_spec_index][1]]);
      printf("      + Transfer Function              : %s\n", VC2DecoderTransferFunctionString[VC2DecoderColorSpecIndices[info.video_format.color_spec_index][2]]);
    }
  }
  printf("\n");
  printf("\n");
  printf("    Picture Coding Mode:\n");
  if (info.picture_coding_mode == VC2DECODER_PCM_FRAME)
    printf("      FRAME\n");
  else
    printf("      FIELD\n");
  printf("\n");
  printf("\n");
  printf("    Transform Parameters:\n");
  printf("      Wavelet                          : %s\n", VC2DecoderWaveletFilterTypeString[info.transform_params.wavelet_index]);
  printf("      Depth                            : %d levels\n", info.transform_params.wavelet_depth);
  printf("      Slices                           : %d x %d\n", info.transform_params.slices_x, info.transform_params.slices_y);
  if (info.transform_params.custom_quant_matrix_flag) {
    printf("      + Custom Quantisation Matrix\n");
    printf("        0 : %4d\n", info.transform_params.quant_matrix_LL);
    for (int l = 1; l < (int)info.transform_params.wavelet_depth; l++) {
      printf("        %1d : %4d  %4d  %4d\n", l, info.transform_params.quant_matrix_LH[l - 1], info.transform_params.quant_matrix_HL[l - 1], info.transform_params.quant_matrix_HH[l - 1]);
    }
  }
  printf("--------------------------------------------------------------------------------\n");
}
