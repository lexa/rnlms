#ifndef _RNLMS_H_
#define _RNLMS_H_

#include <string.h>
#include <stdio.h>
#include <math.h>


#include "global.h"
#include "utils.h"
#include "CircularBuffer.h"
#include "rnlms_interface.h"

/*структура фильтра*/
struct rnlms_data
{
  NUM ALPHA;
  NUM BETTA;
  NUM norma;
  size_t ERR_BUF_LEN;
  size_t len; 
  rnlms_options opt;
  NUM *coeff; /*коэффициенты фильтра*/
  CB* sig; /*кольцевой буфер для хранения послежних len отсчётов входного сигнала*/
  CB* err_buf; /*последние значение ошибок*/
  
}  ;

NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output);

/* #define UNUSED(x) (void)(x) */

#endif /* _RNLMS_H_ */
