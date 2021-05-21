#pragma once

#include <cstdint>
#include "reuss/project_defs.h"
// Force inlining 
#if ( defined(_MSC_VER) || defined(__INTEL_COMPILER) )
#  define STRONG_INLINE __forceinline
#else
#  define STRONG_INLINE inline
#endif

#if defined(__GNUC__)
#  define ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#  define ALWAYS_INLINE STRONG_INLINE
#endif

ALWAYS_INLINE int get_gain(uint16_t raw){
    switch (raw >> 14)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 3:
        return 2;
    default:
        return 0;
    }
}

ALWAYS_INLINE uint16_t get_value(uint16_t raw){
    return raw & ADC_MASK;
}