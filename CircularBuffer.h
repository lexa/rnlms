/*
 * CircularBuffer.h
 *
 *  Created on: 02.05.2011
 *      Author: lexa
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "global.h"

/*тк ccs v3.3 не умеет flexible arrays, то пришлось делать костыли*/
/*#define FLEXIBLE_ARRAY_SIZE 1000*/

typedef struct {
  size_t len;
  size_t begin;/*begin of ring*/
  NUM *data;
} CB;


size_t CB_size(size_t len);
CB* CB_init(void* mem, size_t len);

NUM convolution_CB_and_vector(const CB * const buf, const NUM * const vec);
NUM convolution_CB_and_CB(const CB * const buf0, const CB * const buf1);

void CB_push_elem(CB * const buf, NUM elem);
NUM CB_get_elem(const CB * const buf, size_t pos);
NUM CB_get_first_elem(const CB * const buf);

#endif /* CIRCULARBUFFER_H_ */
