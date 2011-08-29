
#ifndef _RNLMS_H_
#define _RNLMS_H_

#include "global.h"
#include "CircularBuffer.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/*#define FILTER_LEN 300;*/


/*показывает сколько памяти портебуется для хранения фильтра длинны filter_len*/
size_t rlms_sizeOfRequiredMemory(size_t filter_len);

/*инициализирует структуру филтра, по уже выделенной памяти, сама функция ничего не выделяет*/
void* rlms_init(void *mem, NUM BETTA, NUM DELTA, NUM MEMORY_FACTOR, size_t filter_len); 

NUM rlms_func(void*, NUM, NUM, NUM* err, NUM* out);

#endif /* _RNLMS_H_ */


