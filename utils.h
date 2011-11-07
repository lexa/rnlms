#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include "global.h"


/* #define GENERATE_FUNC_convert_ARR_declaration(srcType, dstType, NAME)\ */
/*   void convert_from##_##NAME(srcType srcArr, dstType dstArr, size_t len); */

/* #define GENERATE_FUNC_convert_ARR_definition(srcType, dstType, NAME)  \ */
/* void convert_from##_##NAME(srcType srcArr, dstType dstArr, size_t len)\ */
/* {\ */
/*   size_t i=0;\ */
/*   for (; i<len; ++i)\ */
/*     dstArr[i]=srcArr[i];\ */
/* } */


/* GENERATE_FUNC_convert_ARR_declaration(const NUM*, int16_t*, NUM_to_int16) */
/* GENERATE_FUNC_convert_ARR_declaration(const int16_t*, NUM*, int16_to_NUM) */


#ifdef __GNUC__
#define MIN(a,b)		\
  ({ __typeof__ (a) _a = (a);	\
    __typeof__ (b) _b = (b);	\
    _a < _b ? _a : _b; })
#else
#define MIN(a, b) ((a<b)?a:b);
#endif

#define SIGN(a) ((a<0)?-1:1);

#endif /*_UTILS_H_*/
