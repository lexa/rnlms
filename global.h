#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
typedef double NUM;


typedef struct 
{
	NUM *coeff;
	NUM *sig;
	size_t len;
} SimpleIIRFilter ;

#define NUM_abs(x) fabs(x)

#endif /* _GLOBAL_H_ */

