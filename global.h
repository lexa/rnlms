#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
/*typedef float NUM;*/

#define NUM float

#define NUM_abs(x) fabsf(x)
#define NUM_sqrt(x) sqrtf(x)

NUM sqr (NUM n);
NUM my_sqrtf (NUM n);


#endif /* _GLOBAL_H_ */

