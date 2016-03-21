/*****************************************************************************
 * platform_variant.hpp : code that varies by platform
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

#ifndef __PLATFORM_VARIANT_HPP__
#define __PLATFORM_VARIANT_HPP__

/* Aligned memory allocation */
#ifdef _WIN32
  #define ALIGNED_ALLOC(align, size) _aligned_malloc(size, align)
  #define ALIGNED_FREE(ptr) _aligned_free(ptr)
  #define ALIGNED(N) __declspec(align((N)))
#else
#include <stdlib.h>

static __inline void *vc2_align_malloc(int align, size_t size)
{
    void *ptr = NULL;

    if (posix_memalign(&ptr, align, size)) {
        ptr = NULL;
    }

    return ptr;
}
  #define ALIGNED_ALLOC(align, size) vc2_align_malloc(align, size)
  #define ALIGNED_FREE(ptr) free(ptr)
  #define ALIGNED(N)
#endif

/* __builtin_clz doesn't exist on windows */
#ifdef _WIN32
#include <intrin.h>
#include <cstdint>

static uint32_t __inline __builtin_clz(uint32_t x) {
	unsigned long r = 0;
	_BitScanReverse(&r, x);
	return (31 - r);
}
#endif

static void __inline __detect_cpu_features(bool &HAS_SSE4_2, bool &HAS_AVX, bool &HAS_AVX2) {
#ifdef __GNUC__
  __builtin_cpu_init();

  HAS_SSE4_2 = __builtin_cpu_supports("sse4.2");
  HAS_AVX    = __builtin_cpu_supports("avx");
  HAS_AVX2   = __builtin_cpu_supports("avx2");
#elif _WIN32
  {
    int cpuinfo[4];
    __cpuid(cpuinfo, 0);
    int nids = cpuinfo[0];

    if (nids >= 1) {
      __cpuid(cpuinfo, 1);
      HAS_SSE4_2 = ((cpuinfo[2] & (1 << 20)) != 0);
      HAS_AVX    = ((cpuinfo[2] & (1 << 28)) != 0);

      if (HAS_AVX && nids >= 7) {
        __cpuid(cpuinfo, 7);
        HAS_AVX2 = ((cpuinfo[1] & (1 << 5)) != 0);
      }
    }
  }
#else
  #error Neither Windows nor GCC?!
#endif
}

#endif /* __PLATFORM_VARIANT_HPP__ */
