#ifndef _RNLMS_H_
#define _RNLMS_H_

#include <string.h>
#include <stdio.h>
#include <math.h>


#include "global.h"
#include "utils.h"
#include "CircularBuffer.h"
#include "rnlms_interface.h"

/*структура филтр*/
struct rnlms_data
{
  NUM BETTA;
  NUM DELTA;
  NUM norma;
  NUM MEMORY_FACTOR; //в книге обозначена как α
  size_t len; 
  rnlms_options opt;
  CB* sig; /*кольцевой буфер для хранения послежних len отсчётов входного сигнала*/
  NUM *coeff; /*коэффициенты фильтра*/
  
}  ;

NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output);

/* #define UNUSED(x) (void)(x) */

#endif /* _RNLMS_H_ */
