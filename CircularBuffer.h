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
#include "global.h"

typedef struct {
	size_t len;
	size_t begin;//begin of ring
	NUM data[];
} CB;


size_t CB_size(size_t len);
CB* CB_init(void* mem, size_t len);

NUM convolution_CB_and_vector(const CB * const buf, const NUM * const vec);
void CB_push_elem(CB * const buf, NUM elem);
NUM CB_get_elem(const CB * const buf, size_t pos);


#endif /* CIRCULARBUFFER_H_ */
