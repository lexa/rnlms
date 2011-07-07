
#ifndef _RNLMS_H_
#define _RNLMS_H_

#include "global.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define FILTER_LEN 300;

typedef SimpleIIRFilter* (*InitFilterStruct)(void);

SimpleIIRFilter* rlms_init(void);
NUM rlms_func(SimpleIIRFilter*, NUM, NUM, NUM* err, NUM* out);

#endif /* _RNLMS_H_ */


