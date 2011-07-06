#include "rnlms.h"

#define SIGMA 0.01 // корень из сигмы, несовсем понятно в какую сторону его крутить
#define BETTA 100

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

SimpleIIRFilter* rlms_init(void)
{
	SimpleIIRFilter *rez = malloc (sizeof(SimpleIIRFilter));
	rez->len = FILTER_LEN;
	rez->coeff = malloc (sizeof(NUM) * rez->len);
	rez->sig = malloc (sizeof(NUM) * rez->len);
	int i =0;
	for (; i<rez->len; ++i)
	{
		rez->coeff[i] = 0.0;
		rez->sig[i] = 0.0;
	}
	return rez;
}

NUM filter_output(const SimpleIIRFilter *f)
{
	NUM rez=0.0;
	int i = 0;
	NUM *coeff = f->coeff;
	NUM *sig = f->sig;
	
	for(; i < f->len; ++i, ++coeff, ++sig)
	{
		rez += (*coeff) * (*sig);
	}
	return rez;
}

/* struct error_sample */
/* { */
/* 	double value; */
/* 	int life_time; */
/* }; */


/* NUM median_search(struct error_sample A[], double current_error) */
/* { */
/* 	int i,j,k; */

/* 	current_error=fabs(current_error); */

/* 	for(i=0, j=k=-1; i<P; i++) */
/* 	{ */
/* 		if(A[i].value<current_error) j=i; */
/* 		if(A[i].life_time==0) k=i; */
/* 		A[i].life_time--; */
/* 	} */

/* 	if(k<j) */
/* 	{ */
/* 		for(i=k; i<j; i++) */
/* 			A[i]=A[i+1]; */
/* 		A[j].value=current_error; */
/* 		A[j].life_time=P-1; */
/* 	} */

/* 	if(k>j) */
/* 	{ */
/* 		for(i=k; i>j; i--) */
/* 			A[i]=A[i-1]; */
/* 		A[j+1].value=current_error; */
/* 		A[j+1].life_time=P-1; */

/* 	} */

/* 	if(k==j) */
/* 	{ */
/* 		A[j].value=current_error; */
/* 		A[j].life_time=P-1; */
/* 	} */

/* 	return (P%2)?A[(P-1)/2].value:(A[P/2].value+A[P/2-1].value)/2.0; */
/* } */

NUM calc_norma (const NUM *A, size_t len)
{
	NUM tmp = 0.0;
	int i = 0;
	const NUM *p = A;//указатель на A[i]
	for (; i < len; ++p, ++i)
	{
		tmp += (*p) * (*p);
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

NUM rlms_func(SimpleIIRFilter *f, NUM far, NUM near, NUM *err, NUM *output)
{
	//memmove(f->sig+1, f->sig, (f->len-1)*sizeof(NUM));//сдвигаем коэффициенты вправо
	insert_right(f->sig, far, f->len);

	*output = filter_output(f);
	*err = near - *output;
	NUM norma = calc_norma(f->sig, f->len);
	printf ("far %g norma %g, err %g\n", far, norma, *err);
	
//	printf("%g  %g %g\n", *err, (100+sqrt(norma)), norma);

	for (int i =0; i<f->len; ++i)
	{
		NUM tmp = (NUM_abs(*err)/sqrt(norma));
		if (tmp < SIGMA)
             		f->coeff[i] += (*err)*(f->sig[i]/(BETTA+norma)); 
		else
			f->coeff[i] += SIGMA*SIGN(*err)*(f->coeff[i]/(BETTA+norma));
		
	}
//	printf("\n");
	/* printf("%g %g %g\n", f->coeff[0], f->coeff[f->len/2], f->coeff[f->len-1]); */
	/* printf("%g %g %g\n", f->sig[0], f->sig[f->len/2], f->sig[f->len-1]); */
	return *err;
}
