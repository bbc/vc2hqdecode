/*****************************************************************************
 * dequantise_c.hpp : dequantiser header file: plain C++ version
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

#ifndef __DEQUANTISE_C_CPP__
#define __DEQUANTISE_C_CPP__

#include "common/attributes.h"

#include "dequantise.hpp"

VC2EXPORT DequantiseFunction getDequantiseFunction_c(int slice_width,
                                                     int slice_height,
                                                     int depth,
                                                     int sample_size);
#endif /* __DEQUANTISE_C_CPP__ */
