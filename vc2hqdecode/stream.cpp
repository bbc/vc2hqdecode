/*****************************************************************************
 * stream.cpp : VC2 Stream syntax processing
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

#include "stream.hpp"
#include <cstdlib>
#include <cstdio>

VC2DecoderParseSegment parse_info(char *_data) {
  VC2DecoderParseSegment r;
  uint8_t *data = (uint8_t *)_data;

  if (data[0] != 0x42 || data[1] != 0x42 || data[2] != 0x43 || data[3] != 0x44)
    throw VC2DECODER_NOTPARSEINFO;

  r.parse_code = (data)[4];
  uint32_t next_parse_offset = ((((uint32_t)data[ 5]) << 24) |
                                (((uint32_t)data[ 6]) << 16) |
                                (((uint32_t)data[ 7]) <<  8) |
                                (((uint32_t)data[ 8]) <<  0));
  uint32_t prev_parse_offset = ((((uint32_t)data[ 9]) << 24) |
                                (((uint32_t)data[10]) << 16) |
                                (((uint32_t)data[11]) <<  8) |
                                (((uint32_t)data[12]) <<  0));

  if (next_parse_offset < 13 && next_parse_offset != 0)
    throw VC2DECODER_NOTPARSEINFO;

  r.data = (char *)(data + 13);
  if (next_parse_offset != 0)
    r.next_header = (char *)(data) + next_parse_offset;
  else
    r.next_header = NULL;
  if (prev_parse_offset != 0)
    r.prev_header = (char *)(data) - prev_parse_offset;
  else
    r.prev_header = NULL;
  r.data_length = next_parse_offset - 13;

  return r;
}
