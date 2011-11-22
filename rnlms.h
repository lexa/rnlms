#ifndef _RNLMS_H_
#define _RNLMS_H_

#include <string.h>
#include <stdio.h>
#include <math.h>


#include "global.h"
#include "median.h"
#include "utils.h"
#include "CircularBuffer.h"
#include "rnlms_interface.h"

/*структура фильтра*/
struct rnlms_data
{
  RNLMS_params param;
  rnlms_options opt;
  NUM norma;
  NUM *coeff; /*коэффициенты фильтра*/
  CB* sig; /*кольцевой буфер для хранения послежних len отсчётов входного сигнала*/
  Mediator *err_buf; 
  
}  ;

NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output);

/* #define UNUSED(x) (void)(x) */

#endif /* _RNLMS_H_ */
