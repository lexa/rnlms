#ifndef _RNLMS_H_
#define _RNLMS_H_

#include <string.h>
#include <stdio.h>
#include <math.h>


#include "global.h"
#include "utils.h"
#include "CircularBuffer.h"
#include "rnlms_interface.h"

/*структура не должна торчать наружу*/
struct rnlms_data
{
  NUM BETTA;
  NUM DELTA;
  NUM norma;
  NUM MEMORY_FACTOR;
  size_t len;
  /*	NUM *sig;*/
  CB* sig; /*после инициализации кольцевой буффер лежит после коэффициентов*/
  NUM *coeff;
  
}  ;

/*вычисляет X*X'*/
NUM calc_norma (const NUM *A, size_t len);
void insert_right(NUM *arr, NUM val, size_t len);
NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output);

#define UNUSED(x) (void)(x)

#endif /* _RNLMS_H_ */
