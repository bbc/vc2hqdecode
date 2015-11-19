/*****************************************************************************
 * debug.hpp : controlable debug features
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

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <stdio.h>
#include "logger.hpp"

//#define DEBUG_OP_LENGTHS
//#define DEBUG_OP_TRANSFORMED
//#define DEBUG_P_BLOCK
//#define DEBUG_OP_SLICESIZES
//#define DEBUG_ONE_JOB

#ifdef DEBUG_P_BLOCK
const int DEBUG_P_COMP    = 0;
const int DEBUG_P_BLOCK_Y = 3;
const int DEBUG_P_BLOCK_X = 30;
#endif

#endif /* __DEBUG_HPP__ */
