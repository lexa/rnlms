#include "rnlms.h"

/*#define MEMORY_FACTOR 0.999*/

/*структура не должна торчать наружу*/
typedef struct 
{
  NUM BETTA;
  NUM DELTA;
  NUM norma;
  NUM MEMORY_FACTOR;
  size_t len;
  /*	NUM *sig;*/
  CB* sig; /*после инициализации кольцевой буффер лежит после коэффициентов*/
  NUM *coeff;
} SimpleIIRFilter ;


NUM MIN(NUM a, NUM b)
{
	if (a<b)
		return a;
	else
		return b;
}

NUM SIGN(NUM a)
{
	if (a > 0)
		return 1.0;
	else 
		return -1.0;
}

size_t rlms_sizeOfRequiredMemory(size_t filter_len)
{
	return sizeof(SimpleIIRFilter) +	\
		(sizeof(NUM)*filter_len) +	\
		(CB_size(filter_len)) ;
}

/*инициализирует структуру для фильтра, по уже выделенной памяти*/
void* rlms_init(void *mem, NUM BETTA, NUM DELTA, NUM MEMORY_FACTOR, size_t filter_len)
{
	size_t i;
	SimpleIIRFilter *rez = mem;
	rez->len = filter_len;
	rez->BETTA = BETTA;
	rez->DELTA = DELTA;
	rez->norma = 0.0; 
	rez->MEMORY_FACTOR = MEMORY_FACTOR;
	rez->coeff = (float*)(rez + 1);
	
	rez->sig = CB_init(&(rez->coeff[rez->len]), rez->len);


	i = 0;
	for (; i<rez->len; ++i)
	{
		rez->coeff[i] = 0.0;
		/*		rez->sig[i] = 0.0;*/
	}
	return rez;
}

/*NUM median*/

NUM filter_output(const SimpleIIRFilter *f)
{
	/* NUM rez=0.0; */
	/* int i = 0; */
	/* NUM *coeff = f->coeff; */
	/* NUM *sig = f->sig; */
	
	/* for(; i < f->len; ++i) */
	/* { */
	/* 	rez += f->coeff[i] * f->sig[i]; */
	/* } */
	
	return convolution_CB_and_vector(f->sig, f->coeff);
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
NUM rlms_func(void *f_, NUM far_, NUM near_, NUM *err, NUM *output)
{
	SimpleIIRFilter *f = f_;

	/*	NUM norma = convolution_CB_and_CB(f->sig, f->sig); */
	f->norma += sqr(far_) - sqr(CB_get_elem(f->sig, f->len)) ;

	CB_push_elem(f->sig, far_);

	*output = filter_output(f); 
	*err = near_ - *output;

	/*	fprintf(stderr, "%g\n", f->DELTA);*/

	if ((NUM_abs(*err)/NUM_sqrt(f->norma)) < f->DELTA) 
	{
		NUM tmp = f->BETTA+f->norma;
		size_t i;
		for (i =0; i<f->len; ++i)
		{
			NUM x_i = CB_get_elem(f->sig, i);
             		f->coeff[i] += (*err)*(x_i/tmp);
			
		}
	} else {
		NUM tmp = NUM_sqrt(f->BETTA + f->norma) * SIGN(*err);
		size_t i;
		for (i =0; i<f->len; ++i)
		{
			NUM x_i = CB_get_elem(f->sig, i);
			f->coeff[i] += f->DELTA*(x_i/tmp);
			
		}
	}
	f->DELTA = f->MEMORY_FACTOR * f->DELTA + (1-f->MEMORY_FACTOR) * MIN(sqr(*err)/(f->norma), f->DELTA);


	/*	printf("\n");*/
	/* printf("%g %g %g\n", f->coeff[0], f->coeff[f->len/2], f->coeff[f->len-1]); */
	/* printf("%g %g %g\n", f->sig[0], f->sig[f->len/2], f->sig[f->len-1]); */
	return *err;
}
