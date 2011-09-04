#ifndef _UTILS_H_
#define _UTILS_H_

#include "global.h"

#define GENERATE_FUNC_convert_ARR_declaration(srcType, dstType, NAME)\
  void convert_from##_##NAME(srcType srcArr, dstType dstArr, size_t len);

#define GENERATE_FUNC_convert_ARR_definition(srcType, dstType, NAME)  \
void convert_from##_##NAME(srcType srcArr, dstType dstArr, size_t len)\
{\
  size_t i=0;\
  for (; i<len; ++i)\
    dstArr[i]=srcArr[i];\
}


GENERATE_FUNC_convert_ARR_declaration(const NUM*, int16_t*, NUM_to_int16)
GENERATE_FUNC_convert_ARR_declaration(const int16_t*, NUM*, int16_to_NUM)

NUM MIN(NUM a, NUM b);
size_t MIN_size_t(size_t a, size_t b);


NUM SIGN(NUM a);



#endif /*_UTILS_H_*/
