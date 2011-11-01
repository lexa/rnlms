#include "rnlms.h"

/*#define MEMORY_FACTOR 0.999*/





size_t sizeof_rnlms(size_t filter_len)
{
  return sizeof(struct rnlms_data) + \
    (sizeof(NUM)*filter_len) + \
    (CB_size(filter_len)) ;
}

/*инициализирует структуру для фильтра, по уже выделенной памяти*/
rnlms_result rnlms_init_struct(rnlms_data_hnd mem, NUM BETTA, NUM DELTA, NUM MEMORY_FACTOR, size_t filter_len)
{
  struct rnlms_data* rez = mem;

  if (MEMORY_FACTOR <= 0.0 || MEMORY_FACTOR >= 1.0)
    {
      fprintf(stderr, "MEMORY_FACTOR must be benween 0 and 1\n");
      return E_BAD_MAIN_DATA;
    }

  rez->len = filter_len;
  rez->BETTA = BETTA;
  rez->DELTA = DELTA;
  rez->norma = 0.0; 
  rez->MEMORY_FACTOR = MEMORY_FACTOR;
  rez->opt = 0;
  rez->coeff = (NUM*)(rez + 1);

  
  
  return rnlms_clean_buff(mem);
}


/*вычисляет X*X'*/
NUM calc_norma (const NUM *A, size_t len)
{
  NUM tmp = 0.0;
  size_t i = 0;

  for (; i < len; ++i)
    {
      tmp += sqr(A[i]);
    }
  /*	return sqrt(tmp);*/
  return tmp;
}


void insert_right(NUM *arr, NUM val, size_t len)
{
  size_t i;
  for (i = len-1; i > 0; --i)
    {
      arr[i] = arr[i-1];
    }
  arr[0]=val;
}


/*выполняет для адаптацию*/
NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output)
{
  size_t i;

  /*	NUM norma = convolution_CB_and_CB(f->sig, f->sig); */
  f->norma += sqr(near_) - sqr(CB_get_first_elem(f->sig)) ;

  /* fprintf(stderr, "%g | %g\n", CB_get_first_elem(f->sig), CB_get_elem(f->sig, f->sig->len));  */
  /* assert (CB_get_first_elem(f->sig) == CB_get_elem(f->sig, f->sig->len)); */

  CB_push_elem(f->sig, near_);

  /* *output = filter_output(f); */
  *output = convolution_CB_and_vector(f->sig, f->coeff);
  *err = far_ - *output;

  /*	fprintf(stderr, "%g\n", f->DELTA);*/
  
  if (f->opt & OPT_INHIBIT_ADAPTATION)
    return *err;

  if ((NUM_abs(*err)/NUM_sqrt(f->norma)) < f->DELTA)
    {
      NUM tmp = f->BETTA+f->norma;
      size_t i;
      for (i =0; i<f->len; ++i)
	{
	  NUM x_i = CB_get_elem(f->sig, i);
	  f->coeff[i] += (*err)*(x_i/tmp);
	  fprintf(stderr, "NLMS %g\n", f->DELTA);
	}
    } else {
    NUM tmp = NUM_sqrt(f->BETTA + f->norma) * SIGN(*err);
    
    for (i =0; i<f->len; ++i)
      {
	NUM x_i = CB_get_elem(f->sig, i);
	f->coeff[i] += f->DELTA*(x_i/tmp);
	fprintf(stderr, "delta %g\n", f->DELTA);
			
      }
  }
  f->DELTA = f->MEMORY_FACTOR * f->DELTA + (1 - (f->MEMORY_FACTOR)) * MIN(sqr(*err)/(f->norma), f->DELTA);
	

  /*	printf("\n");*/
  /* printf("%g %g %g\n", f->coeff[0], f->coeff[f->len/2], f->coeff[f->len-1]); */
  /* printf("%g %g %g\n", f->sig[0], f->sig[f->len/2], f->sig[f->len-1]); */
  return *err;
}

rnlms_result rnlms_process(rnlms_data_hnd rnlms_hnd, 
			   const int16_t *x_arr,        // far abonent signal
			   const int16_t *y_arr,        // near abonent signal
			   int16_t *err_out,    // result with reduced echo
			   size_t size)
{
  size_t i;
  for(i=0; i<size; i++, x_arr++, y_arr++, err_out++)
    {
      NUM far_, near_, err_, out_; 
      far_= *x_arr;
      near_= *y_arr;
      rnlms_func(rnlms_hnd, far_, near_, &err_, &out_);
      *err_out=err_ ;
    }
  return E_NO_ERROR;
}

/* просто LMS */
/* NUM rnlms_func(void *f_, NUM far_, NUM near_, NUM *err, NUM *output) */
/* { */
/* 	SimpleIIRFilter *f = f_; */
	
/* 	int i; */

/* 	f->norma += sqr(far_) - sqr(CB_get_first_elem(f->sig)) ; */

/* 	CB_push_elem(f->sig, far_); */


/* 	*output = convolution_CB_and_vector(f->sig, f->coeff); */
/* 	*err = near_ - *output; */

/* 	NUM tmp = f->BETTA+f->norma; */
/* 	for (i =0; i<f->len; ++i) */
/* 	  { */
/* 	    NUM x_i = CB_get_elem(f->sig, i); */
/* 	    f->coeff[i] += (*err)*(x_i/tmp); */
/* 	  } */

	

/* 	return *err; */
/* } */



rnlms_result rnlms_clean_buff(rnlms_data_hnd rez)
{
  size_t i;
  memset(rez->coeff, 0, sizeof(NUM)*(rez->len));
  rez->sig = CB_init(&(rez->coeff[rez->len]), rez->len);

  i = 0;
  for (; i<rez->len; ++i)
    {
      rez->coeff[i] = 0.0;
    }
  return E_NO_ERROR;
  
}

/* size_t sizeof_rnlms_2(float BETTA, float DELTA, float MEMORY_FACTOR, size_t filter_len) */
/* //size_t sizeof_rnlms_2(float, float, float, size_t filter_len) */
/* { */
/*   UNUSED(BETTA); */
/*   UNUSED(DELTA); */
/*   UNUSED(MEMORY_FACTOR); */

/*   return sizeof_rnlms(filter_len); */
/* } */


rnlms_options rnlms_get_options(const rnlms_data_hnd mem)
{
  return (mem->opt);
}

rnlms_result rnlms_set_options(rnlms_data_hnd mem, rnlms_options new_opt)
{
  mem->opt = new_opt;
  return E_NO_ERROR;
}
