/*
 * CircularBuffer.h
 *
 *  Created on: 02.05.2011
 *      Author: lexa
 */


#include "CircularBuffer.h"

size_t
CB_calc_pos(const CB * const buf, size_t pos)
{

  /* этот вариант может сгодится если мы не любим делить */
  /* size_t tmp = buf->begin + pos; */
		
  /* while (tmp >= buf->len) */
  /* { */
  /* 	tmp -= buf->len; */
  /* } */
  /* return tmp; */
	
  return (buf->begin + pos + buf->len)%(buf->len); /*если длина буфера 2^n то можно тут сэкономить*/
}

NUM
convolution_CB_and_CB(const CB * const buf0, const CB * const buf1)
{
  size_t i;
  NUM rez;
  assert(buf0->len != buf1->len);
  i = 0;
  rez = 0.0;
  for (; i<buf0->len; ++i)
    {
      rez += buf0->data[i] * buf1->data[i];
    }
  return rez;
}

NUM
convolution_CB_and_vector(const CB * const buf, const NUM * const vec)
{
  /* size_t i; */
  /* NUM rez=0.0f; */
  /* for (i=0; i < buf->len; ++i) */
  /* { */
  /* 	rez += CB_get_elem(buf, i) * vec[i]; */
  /* } */
  /* return rez; */

  size_t i;
  NUM rez = 0.0;
	
  for (i=buf->begin; i<buf->len; ++i)
    {
      rez += buf->data[i] * vec[i-buf->begin];
    }

  for (i=0; i<buf->begin; ++i)
    {
      rez += buf->data[i] * vec[i+buf->len-buf->begin];
    }
  return rez;
}

size_t CB_size(size_t len)
{
  NUM s[1];
  return sizeof(CB) + \
    sizeof(s[0])*len;
}


CB*
CB_init(void* mem, size_t len)
{
  size_t i;
  CB* buf=mem;
  buf->len = len;
  buf->begin = 0;
  buf->data = (float*)(&buf+1); /*указатель на память после структуры*/
  
  
  for ( i = 0; i<buf->len; ++i)
    {
      buf->data[i] = 0.0;
    }
  return buf;
}

void
CB_push_elem(CB * const buf, NUM elem)
{
  buf->data[buf->begin] = elem;
  buf->begin = CB_calc_pos(buf, 1);
}

NUM
CB_get_elem(const CB * const buf, size_t pos)
{
  return buf->data[CB_calc_pos(buf, pos)];
}


