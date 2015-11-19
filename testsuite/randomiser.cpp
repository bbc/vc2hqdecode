/*****************************************************************************
 * randomiser.cpp : generate some random input data
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

#ifdef __linux__

#include <cstdint>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "randomiser.hpp"

bool randomiser(char *out, int _length) {
  int f = open("/dev/urandom", O_RDONLY);
  ssize_t length = _length;
  int offs = 0;
  while (length > 0) {
    ssize_t s = read(f, ((uint8_t *)out) + offs, length);
    if (s <= 0) {
      return false;
    }
    offs += s;
    length -= s;
  }

  return true;
}

#else

#include <random>

static std::mt19937 generator;

bool randomiser(char *out, int length) {
  for (int i = 0; i < length/4; i++) {
    ((uint32_t*)out)[i] = generator();
  }

  return true;
}


#endif
