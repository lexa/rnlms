#include "utils.h"

/* NUM SIGN(NUM a) */
/* { */
/* 	if (a > 0) */
/* 		return 1.0; */
/* 	else */
/* 		return -1.0; */
/* } */

/* NUM MIN(NUM a, NUM b) */
/* { */
/*   return (a<b)?a:b; */
/* } */

/* size_t MIN_size_t(size_t a, size_t b) */
/* { */
/*   return (a<b)?a:b; */
/* } */

GENERATE_FUNC_convert_ARR_definition(const NUM*, int16_t*, NUM_to_int16)
GENERATE_FUNC_convert_ARR_definition(const int16_t*, NUM*, int16_to_NUM)
