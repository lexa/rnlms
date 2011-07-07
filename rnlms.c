#include "rnlms.h"


//структура не должна торчать наружу
typedef struct 
{
	NUM BETTA;
	NUM SIGMA;
	size_t len;
//	NUM *sig;
	CB* sig; //надо инициализировать самому
	NUM coeff[];
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
		(sizeof(NUM[filter_len])) +	\
		(CB_size(filter_len)) ;
}

//инициализирует структуру для фильтра, по уже выделенной памяти
void* rlms_init(void *mem, NUM BETTA, NUM SIGMA, size_t filter_len)
{
	SimpleIIRFilter *rez = mem;
	rez->len = filter_len;
	rez->BETTA = BETTA;
	rez->SIGMA = SIGMA;

	// rez->coeff этоflexible array member
	
	rez->sig = CB_init(&rez->coeff[rez->len], rez->len);


	int i = 0;
	for (; i<rez->len; ++i)
	{
		rez->coeff[i] = 0.0;
//		rez->sig[i] = 0.0;
	}
	return rez;
}

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

//вычисляет X*X'
NUM calc_norma (const NUM *A, size_t len)
{
	NUM tmp = 0.0;
	int i = 0;
//	const NUM *p = A;//указатель на A[i]
	for (; i < len; ++i)
	{
		tmp += A[i] * A[i];
	}
//	return sqrt(tmp);
	return tmp;
}


void insert_right(NUM *arr, NUM val, size_t len)
{
	for (size_t i = len-1; i > 0; --i)
	{
		arr[i] = arr[i-1];
	}
	arr[0]=val;
}


//выполняет для адаптацию
NUM rlms_func(void *f_, NUM far, NUM near, NUM *err, NUM *output)
{
	SimpleIIRFilter *f = f_;
	//memmove(f->sig+1, f->sig, (f->len-1)*sizeof(NUM));//сдвигаем коэффициенты вправо
	//insert_right(f->sig, far, f->len);
	CB_push_elem(f->sig, far);

	*output = filter_output(f);
	*err = near - *output;
//	NUM norma = calc_norma(f->sig, f->len);
	const NUM norma = convolution_CB_and_CB(f->sig, f->sig);
	printf ("far %g norma %g, err %g\n", far, norma, *err);
	
//	printf("%g  %g %g\n", *err, (100+sqrt(norma)), norma);

	if ((NUM_abs(*err)/sqrt(norma)) < f->SIGMA) 
	{
		NUM tmp = f->BETTA+norma;
		for (size_t i =0; i<f->len; ++i)
		{
			NUM x_i = CB_get_elem(f->sig, i);
             		f->coeff[i] += (*err)*(x_i/tmp);
			
		}
	} else {
		NUM tmp = sqrt(f->BETTA+norma) * SIGN(*err);
		for (size_t i =0; i<f->len; ++i)
		{
			NUM x_i = CB_get_elem(f->sig, i);
			f->coeff[i] += f->SIGMA*(x_i/tmp);
			
		}
	}
//	printf("\n");
	/* printf("%g %g %g\n", f->coeff[0], f->coeff[f->len/2], f->coeff[f->len-1]); */
	/* printf("%g %g %g\n", f->sig[0], f->sig[f->len/2], f->sig[f->len-1]); */
	return *err;
}
