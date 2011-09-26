
#ifndef _RNLMS_H_
#define _RNLMS_H_

#include <string.h>
#include <stdio.h>
#include <math.h>


#include "global.h"
#include "utils.h"
#include "CircularBuffer.h"

/*#define FILTER_LEN 300;*/


/*показывает сколько памяти портебуется для хранения фильтра длинны filter_len*/
size_t rnlms_sizeOfRequiredMemory(size_t filter_len);

/*инициализирует структуру филтра, по уже выделенной памяти, сама функция ничего не выделяет*/
void* rnlms_init(void *mem, NUM BETTA, NUM DELTA, NUM MEMORY_FACTOR, size_t filter_len); 

NUM rnlms_func(void*, NUM, NUM, NUM* err, NUM* out);

#endif /* _RNLMS_H_ */


