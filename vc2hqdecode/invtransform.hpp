/*****************************************************************************
 * invtransform.hpp : inverse transform
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

#ifndef __INVTRANSFORM_HPP__
#define __INVTRANSFORM_HPP__

#include "internal.h"

typedef void (*InplaceTransform)(void *idata,
                                 const int istride,
                                 const int width,
                                 const int height);

typedef void (*InplaceTransformFinal)(void *idata,
                                      const int istride,
                                      const char *odata,
                                      const int ostride,
                                      const int iwidth,
                                      const int iheight,
                                      const int ooffset_x,
                                      const int ooffset_y,
                                      const int owidth,
                                      const int oheight);

typedef InplaceTransform (*GetInvVTransform)(int wavelet_index, int level, int depth, int sample_size);
typedef InplaceTransform (*GetInvHTranform)(int wavelet_index, int level, int depth, int sample_size);
typedef InplaceTransformFinal (*GetInvHTransformFinal)(int wavelet_index, int active_bits, int sample_size);

#endif /* __INVTRANSFORM_HPP__ */
