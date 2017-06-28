/*****************************************************************************
 * vc2hqdecode.h : External interface
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

#ifndef __VC2HQDECODE_H__
#define __VC2HQDECODE_H__

#include <stdint.h>

#if defined _WIN32 || defined __CYGWIN__
  #define VC2HQDECODE_EXPORT __declspec(dllexport)
  #define VC2HQDECODE_IMPORT __declspec(dllimport)
#else
  #if __GNUC__ >= 4
    #define VC2HQDECODE_EXPORT __attribute__((visibility("default")))
    #define VC2HQDECODE_IMPORT __attribute__((visibility("default")))
  #else
    #define VC2HQDECODE_EXPORT
    #define VC2HQDECODE_IMPORT
  #endif
#endif

#ifdef VC2HQDECODE_DLL
#define VC2HQDECODE_API VC2HQDECODE_EXPORT
#else
#define VC2HQDECODE_API VC2HQDECODE_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This constant will change when the API in this header file changes.
 */
#define VC2DECODER_API_VERSION 2

/*
 This forces a link error if trying to link to an incompatible version of the code,
 the actual call to vc2decode_init becomes a call to vc2decode_init_##VC2DECODER_API_VERSION
 */
#define vc2decoder_combine_internal(x,y) x##y
#define vc2decoder_combine(x,y) vc2decoder_combine_internal(x,y)
#define vc2decode_init vc2decoder_combine(vc2decode_init_,VC2DECODER_API_VERSION)

/**
 * This type encapsulates a decoder which has been created.
 */
typedef void * VC2DecoderHandle;

/**
 * This structre is used to configure the user configurable parameters for a decoder.
 */
typedef struct _VC2DecoderParamsUser {
  /**
   * set the number of threads the decoder will use internally. The current implementation does not support
   * numbers of threads greater than 16. The decoded is aggressively multithreaded and will distribute work
   * over as many threads as it is given. It is generally extremely efficient at doing so, but more threads
   * does increase the total workload needed to decode a frame (due to some duplication being needed) and
   * so it is recommended that the number of threads be kept as low as will work -- which may require some
   * experimentation to determine.
   */
  int threads;

  /**
   * This sets the numa number of the first CPU core to which decoder threads should set their affinity.
   */
  int numa_first_node;


  /**
   * These are debugging settings which will recolourise the output based on properties of the stream.
   */
  int colourise_quantiser;
  int colourise_padding;
  int colourise_unpadded;

  /**
   * If this is set to non-zero then a specific offset and dimensions can be set for the decoded output
   * These must define a rectangle within the actual decoded picture, which will be clipped aporpriately.
   * This is useful for decoding only some segments of a large image.
   */
  int partial_decode;
  int partial_decode_offset_x;
  int partial_decode_offset_y;
  int partial_decode_width;
  int partial_decode_height;
} VC2DecoderParamsUser;



/*******************************************************************************
 *
 *                             Stream Metadata
 *
 *******************************************************************************/

enum _VC2DecoderSubsamplingFormat {
  VC2DECODER_CDS_420 = 0,
  VC2DECODER_CDS_422 = 1,
  VC2DECODER_CDS_444 = 2
};

enum _VC2DecoderFrameRate {
  VC2DECODER_FR_CUSTOM = 0,
  VC2DECODER_FR_24000_1001 = 1,
  VC2DECODER_FR_24 = 2,
  VC2DECODER_FR_25 = 3,
  VC2DECODER_FR_30000_1001 = 4,
  VC2DECODER_FR_30 = 5,
  VC2DECODER_FR_50 = 6,
  VC2DECODER_FR_60000_1001 = 7,
  VC2DECODER_FR_60 = 8,
  VC2DECODER_FR_15000_1001 = 9,
  VC2DECODER_FR_25_2 = 10,
  VC2DECODER_FR_48 = 11
};

enum _VC2DecoderPresetSignalRange {
  VC2DECODER_PSR_CUSTOM   = 0,
  VC2DECODER_PSR_8BITFULL = 1,
  VC2DECODER_PSR_8BITVID  = 2,
  VC2DECODER_PSR_10BITVID = 3,
  VC2DECODER_PSR_12BITVID = 4
};

enum _VC2DecoderBaseVideoFormat {
  VC2DECODER_BVF_CUSTOM     = 0,
  VC2DECODER_BVF_QSIF525    = 1,
  VC2DECODER_BVF_QCIF       = 2,
  VC2DECODER_BVF_SIF525     = 3,
  VC2DECODER_BVF_CIF        = 4,
  VC2DECODER_BVF_4SIF525    = 5,
  VC2DECODER_BVF_4CIF       = 6,
  VC2DECODER_BVF_SD480I_60  = 7,
  VC2DECODER_BVF_SD576I_50  = 8,
  VC2DECODER_BVF_HD720P_60  = 9,
  VC2DECODER_BVF_HD720P_50  = 10,
  VC2DECODER_BVF_HD1080I_60 = 11,
  VC2DECODER_BVF_HD1080I_50 = 12,
  VC2DECODER_BVF_HD1080P_60 = 13,
  VC2DECODER_BVF_HD1080P_50 = 14,
  VC2DECODER_BVF_DC2K       = 15,
  VC2DECODER_BVF_DC4K       = 16,
  VC2DECODER_BVF_UHDTV4K_60 = 17,
  VC2DECODER_BVF_UHDTV4K_50 = 18,
  VC2DECODER_BVF_UHDTV8K_60 = 19,
  VC2DECODER_BVF_UHDTV8K_50 = 20,
  VC2DECODER_BVF_HD1080P_24 = 21,
  VC2DECODER_BVF_SDPRO486   = 22,

  VC2DECODER_BVF_NUM
};

enum _VC2DecoderPictureCodingMode {
  VC2DECODER_PCM_FRAME = 0,
  VC2DECODER_PCM_FIELD = 1,
};

enum _VC2DecoderWaveletFilterType {
  VC2DECODER_WFT_DESLAURIERS_DUBUC_9_7  = 0,
  VC2DECODER_WFT_LEGALL_5_3             = 1,
  VC2DECODER_WFT_DESLAURIERS_DUBUC_13_7 = 2,
  VC2DECODER_WFT_HAAR_NO_SHIFT          = 3,
  VC2DECODER_WFT_HAAR_SINGLE_SHIFT      = 4,
  VC2DECODER_WFT_FIDELITY               = 5,
  VC2DECODER_WFT_DAUBECHIES_9_7         = 6,

  VC2DECODER_WFT_NUM
};

enum _VC2DecoderPixelAspect {
  VC2DECODER_PAR_CUSTOM = 0,
  VC2DECODER_PAR_1_1 = 1,
  VC2DECODER_PAR_10_11 = 2,
  VC2DECODER_PAR_12_11 = 3,
  VC2DECODER_PAR_40_33 = 4,
  VC2DECODER_PAR_16_11 = 5,
  VC2DECODER_PAR_4_3 = 6,
};

enum _VC2DecoderColorSpec {
  VC2DECODER_CSP_CUSTOM  = 0,
  VC2DECODER_CSP_SDTV525 = 1,
  VC2DECODER_CSP_SDTV625 = 2,
  VC2DECODER_CSP_HDTV    = 3,
  VC2DECODER_CSP_DCINE   = 4
};

enum _VC2DecoderColorPrimaries {
  VC2DECODER_CPR_HDTV    = 0,
  VC2DECODER_CPR_SDTV525 = 1,
  VC2DECODER_CPR_SDTV625 = 2,
  VC2DECODER_CPR_DCINE   = 3
};

enum _VC2DecoderColorMatrix {
  VC2DECODER_CMA_HDTV       = 0,
  VC2DECODER_CMA_SDTV       = 1,
  VC2DECODER_CMA_REVERSIBLE = 2,
  VC2DECODER_CMA_RGB        = 3
};

enum _VC2DecoderTransferFunction {
  VC2DECODER_TRF_TVGAMMA  = 0,
  VC2DECODER_TRF_EXTGAMUT = 1,
  VC2DECODER_TRF_LINEAR   = 2,
  VC2DECODER_TRF_DCINE    = 3
};

typedef struct _VC2DecoderVideoFormat {
  uint32_t base_video_format;


  int custom_dimensions_flag;
  uint32_t frame_width;
  uint32_t frame_height;


  int custom_color_diff_format_flag;
  uint32_t color_diff_format_index;


  int custom_scan_format_flag;
  uint32_t source_sampling;


  int custom_frame_rate_flag;
  uint32_t frame_rate_index;
  uint32_t frame_rate_numer;
  uint32_t frame_rate_denom;


  int custom_pixel_aspect_ratio_flag;
  uint32_t pixel_aspect_ratio_index;
  uint32_t pixel_aspect_ratio_numer;
  uint32_t pixel_aspect_ratio_denom;


  int custom_clean_area_flag;
  uint32_t clean_width;
  uint32_t clean_height;
  uint32_t left_offset;
  uint32_t top_offset;


  int custom_signal_range_flag;
  uint32_t signal_range_index;
  uint32_t luma_offset;
  uint32_t luma_excursion;
  uint32_t color_diff_offset;
  uint32_t color_diff_excursion;

  int custom_color_spec_flag;
  uint32_t color_spec_index;

  int custom_color_primaries_flag;
  uint32_t color_primaries_index;

  int custom_color_matrix_flag;
  uint32_t color_matrix_index;

  int custom_transfer_function_flag;
  uint32_t transfer_function_index;
} VC2DecoderVideoFormat;

#define MAX_DWT_DEPTH 8

typedef struct _VC2DecoderTransformParams {
  uint32_t wavelet_index;
  uint32_t wavelet_depth;

  uint32_t slices_x;
  uint32_t slices_y;

  int custom_quant_matrix_flag;
  uint32_t quant_matrix_LL;
  uint32_t quant_matrix_HL[MAX_DWT_DEPTH];
  uint32_t quant_matrix_LH[MAX_DWT_DEPTH];
  uint32_t quant_matrix_HH[MAX_DWT_DEPTH];

  /* V3.0 specific entries:
     These entries will always be set to 0
     for streams with major_version < 3 */
  int asym_transform_index_flag;
  uint32_t wavelet_index_ho;

  int asym_transform_flag;
  uint32_t wavelet_depth_ho;
} VC2DecoderTransformParams;

/**
 * This structure returns metadata regarding the stream. Including
 * the full details decoded from the most recent Sequence Header and
 * picture.
 */
typedef struct _VC2DecoderSequenceInfo {
  VC2DecoderVideoFormat     video_format;
  uint32_t picture_coding_mode;
  VC2DecoderTransformParams transform_params;

  int sequence_headers_seen;
  int pictures_decoded;
  unsigned int last_picture_number;
} VC2DecoderSequenceInfo;








/**
 * This structure contains details of the output format which the decoder will produce
 * It is the responsibility of the host application to take these values into account
 * when allocating memory for the decoded pictures to be written into.
 */
typedef struct _VC2DecoderOutputFormat {
  /** The width of the entire frame in pixels */
  int width;

  /** The height of the entire frame in pixels */
  int height;

  /** One of the values in VC2DecoderPresetSignalRange */
  int signal_range;

  /** One of ther values in VC2DecoderSubsamplingFormat */
  int source_sampling;

  /** The frame rate numerator and denominator */
  int frame_rate_numer;
  int frame_rate_denom;

  /** Non-zero if the output format is interlaced */
  int interlaced;
} VC2DecoderOutputFormat;


/**
 * Defined values for return codes from the library functions. Success codes are
 * greater than or equal to 0, errors are less than or equal to zero.
 */
typedef enum _VC2DecoderResult {
  /** Success */
  VC2DECODER_OK = 0,

  /** Success and End of Stream Encountered */
  VC2DECODER_OK_EOS             = 1,
  /** Success and Decoder Reconfigured -- call vc2decode_get_output_format next */
  VC2DECODER_OK_RECONFIGURED    = 2,
  /** Success and picture decoded */
  VC2DECODER_OK_PICTURE         = 3,
  /** Success and Auxiliary Data Unit found -- call vc2decode_extract_aux next */
  VC2DECODER_OK_AUXILIARY       = 4,
  /** Success but picture format not supported by decoder */
  VC2DECODER_OK_INVALID_PICTURE = 5,

  /** Error: Paramters are bad */
  VC2DECODER_BADPARAMS      = -1,
  /** Error: Decode Failed */
  VC2DECODER_DECODE_FAILED  = -2,
  /** Error: Threading Problem */
  VC2DECODER_BADTHREAD      = -3,
  /** Error: Feature not yet implemented */
  VC2DECODER_NOTIMPLEMENTED = -4,
  /** Error: Invalid Quantiser value  */
  VC2DECODER_NOQUANTISER    = -5,
  /** Error: Decoder Overran input data */
  VC2DECODER_CODEROVERRUN   = -6,
  /** Error: No Parse Info Header Found */
  VC2DECODER_NOTPARSEINFO   = -7,
  /** Error: Stream Syntax Problem */
  VC2DECODER_BADSTREAM      = -8,

  /** Error */
  VC2DECODER_UNKNOWN_ERROR  = -99
} VC2DecoderResult;


/**
 * This structure is used to set up logging for the library. The functions provided will be used to
 * log messages by the decoder.
 *
 * Each such function will be called with a character string containing a message as the first parameter.
 * The second parameter will be the same value which was supplied as the "opaque" entry in this structure.
 * Not all functions need be provided. Any pointer set to NULL will not be used.
 *
 * The default behaviour if no logger functions are provided will be to log to standard error.
 */
typedef struct _VC2DecoderLoggers {
  void (*error)(char *, void *);
  void (*warn)(char *, void *);
  void (*info)(char *, void *);
  void (*debug)(char *, void *);
  void *opaque;
} VC2DecoderLoggers;


/*---------------------------------*/
/* Methods in interface of library */
/*---------------------------------*/


/*---------------------------------*/
/* Lifecycle control */
/*---------------------------------*/
/**
 * This function must be the first function from this interface called
 * and must be called only once. Internally it performs certain runtime
 * detection of CPU features in order to determine which versions of
 * certain functions to make use of
 */
VC2HQDECODE_API void vc2decode_init(void);


/**
 * This function can be used to setup logging for the library. This call
 * is global for the entire application and should generally only be called
 * once before any encoders are created.
 *
 * The paramter is a struct of type VC2DecoderLoggers
 */
VC2HQDECODE_API void vc2decoder_init_logging(VC2DecoderLoggers);

/**
 * This function is used by the host application to create a new decoder, which
 * will be represented by the opaque VC2DecoderHandle data type. A single
 * application can create multiple decoders at once if it needs to but in
 * general this is not likely to be required unless decoding multiple streams
 * in parallel.
 */
VC2HQDECODE_API VC2DecoderHandle vc2decode_create(void);


/**
 * This function is used by the host application to destroy a decoder that has been
 * created.
 */
VC2HQDECODE_API void vc2decode_destroy(VC2DecoderHandle);

/*---------------------------------*/
/* Configuration                   */
/*---------------------------------*/
/**
 * This function is used to configure a created decoder. In normal
 * useage it should not be called often. The VC2DecoderParamsUser
 * is described at its definition.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_set_parameters(VC2DecoderHandle handle, VC2DecoderParamsUser params);

/*---------------------------------*/
/* Decode Operation                */
/*---------------------------------*/
/**
 * This is a stream processing function which should be called at the
 * start of the decoding of a sequence and will cause the decoder to parse
 * through the input sequence looking for the first recognisable sequence header
 * and then reconfigure itself based on it.
 *
 * Paramaters:
 *   idata:    the address of a pointer to the input data.
 *   ilength:  the length of the input data in bytes.
 *   skip_aux: if non-zero then Auxiliary Data Units will be ignored and treated as padding.
 *
 * Return Values:
 *   VC2DECODER_OK_RECONFIGURED: The decoder has been reconfigured, and vc2decode_get_output_format
 *     should be called.
 *   VC2DECODER_OK_AUXILIARY: An axuiliary data unit has been detected and vc2decode_extract_aux
 *     should be called.
 *   VC2DECODER_OK_EOS: The stream ended before a sequence header was detected.
 *
 * In all cases the pointer addressed by idata will be advanced and the new value may be used for
 * further stream processing functions. The decoder does not take responsibility for the memory in
 * idata, the host program remains responsible for freeing it when it needs to.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_synchronise(VC2DecoderHandle handle, char **idata, int ilength, int skip_aux);

/**
 * This function is used to extract information on the output format the decoder will write into. It should
 * be called each time a stream processing function returns VC2DECODER_OK_RECONFIGURED, before the next call
 * to  vc2decode_decode_one_picture.
 *
 * Paramaters:
 *   fmt: the address of a VC2DecoderOutputFormat structure
 *
 * Return Values:
 *   VC2DECODER_OK: No error occurred.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_get_output_format(VC2DecoderHandle handle, VC2DecoderOutputFormat *fmt);

/**
 * This function is used to actually decode a picture. It parses the stream searching for a recognisable
 * sequence header or an HQ picture to decode (and optionally auxiliary data units). In usual operation
 * it will be called repeatedly. If a picture is found it will be decoded and the function will not return
 * until it has been fully decoded (or an error has occured).
 *
 * Management of the output data is the responsibility of the host application. The size and format of this data
 * are indicated in the VC2DecoderOutputFormat structure obtained from a call to vc2decode_get_output_format.
 *
 * Paramaters:
 *   idata:    the address of a pointer to the input data.
 *   ilength:  the length of the inpur data.
 *   odata:    an array of three pointers to the output data, one each to the Y, Cb, and Cr components.
 *   ostride:  an array of three integers indicating the offset in SAMPLES from the start of one line to the start of
 *             the next in the output data.
 *   skip_aux: if non-zero then Auxiliary Data Units will be ignored and treated as padding.
 *
 * Return Values:
 *   VC2DECODER_OK_PICTURE: A picture has been succesfully decoded.
 *   VC2DECODER_OK_EOS:  End of Sequence Encountered.
 *   VC2DECODER_OK_RECONFIGURED: The decoder has reconfigured itself, and vc2decode_get_output_format should be called.
 *   VC2DECODER_OK_AUXILIARY: Auxiliary Data present, vc2decode_extract_aux should be called.
 *   VC2DECODER_OK_INVALID_PICTURE: The decoder has encountered a non-HQ picture, but the stream is otherwise valid.
 *
 * In all cases the pointer addressed by idata will be advanced and the new value may be used for
 * further stream processing functions. The decoder does not take responsibility for the memory in
 * idata, the host program remains responsible for freeing it when it needs to.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_decode_one_picture(VC2DecoderHandle handle, char **idata, int ilength, uint16_t **odata, int *ostride, int skip_aux);

/**
 * This function is used to extract metadata from the decoder, which will be accurate at the time it is extracted. This includes
 * all the information present in the most recent VC-2 Sequence Header.
 *
 * Paramaters:
 *   info: the address of a VC2DecoderSequenceInfo structure in which useful information will be returned
 *
 * Return Values:
 *   VC2DECODER_OK: No error occurred.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_sequence_info(VC2DecoderHandle handle, VC2DecoderSequenceInfo *info);


/**
 * This function should be called when a previous call has returned VC2DECODER_OK_AUXILIARY and is used to
 * extract Auxiliary dat from the stream.
 *
 * Paramaters:
 *   idata:    the address of a pointer to the input data.
 *   ilength:  the length of the input data.
 *   odata:    the address of a pointer which will be populated with the location of the auxiliary data
 *   olength:  the address of a variable in which the length of the auxiliary data will be stored.
 *
 * Return Values:
 *   VC2DECODER_OK: No error occurred.
 *
 * In all cases the pointer addressed by idata will be advanced and the new value may be used for
 * further stream processing functions. The decoder does not take responsibility for the memory in
 * idata, the host program remains responsible for freeing it when it needs to.
 *
 * The address placed in odata is not newly allocated memory, and in fact points to a location within the memory block
 * which was originally addressed by idata. The host application should bear this in mind for memory management.
 */
VC2HQDECODE_API VC2DecoderResult vc2decode_extract_aux(VC2DecoderHandle handle, char **idata, int ilength, uint8_t **odata, int *olength);

#ifdef __cplusplus
};
#endif

#endif /* __VC2HQDECODE_H__ */
