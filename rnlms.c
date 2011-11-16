#include "rnlms.h"

size_t sizeof_rnlms(size_t P, size_t filter_len)
{
  return sizeof(struct rnlms_data) + \
    (sizeof(NUM)*filter_len) + \
    (CB_size(filter_len)) + 
    (CB_size(P));
}

/*
инициализирует структуру для фильтра, по уже выделенной памяти
*/
rnlms_result rnlms_init_struct(rnlms_data_hnd mem, NUM ALPHA, NUM BETTA, size_t ERR_BUF_LEN, size_t filter_len)
{
  struct rnlms_data* rez = mem;
  
 
  rez->len = filter_len;
  rez->ALPHA = ALPHA;
  rez->BETTA = BETTA;
  rez->ERR_BUF_LEN = ERR_BUF_LEN;
  rez->norma = 0.0; 
  //  rez->MEMORY_FACTOR = MEMORY_FACTOR;
  rez->opt = 0;
  rez->coeff = (NUM*)(rez + 1);

  return rnlms_clean_buff(mem);
}


/*
Обрабатывает один отсчёт сигнала:
1) расчитывает выход фильтра
2) адаптирует коэффициенты фильтра
*/
NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output)
{
  size_t i;


  f->norma += SQR(near_) - SQR(CB_get_first_elem(f->sig)) ;//  f->norma == X*X' 

  CB_push_elem(f->sig, near_);

  *output = convolution_CB_and_vector(f->sig, f->coeff); //выход фильтра 
  *err = far_ - *output; 

  CB_push_elem(f->err_buf, *err);
  
  if (f->opt & OPT_INHIBIT_ADAPTATION)
    return *err;


  NUM Psi;

  NUM mediana = CB_mediana(f->err_buf);

  if (3 * fabs(mediana) > *err) //FIXME почему 3 ???
    Psi = *err * (f->ALPHA);
  else
    Psi = 0.5f * mediana * (f->ALPHA) * SIGN(*err); 

  
  for (i =0; i<f->len; ++i)
    {
      NUM x_i = CB_get_elem(f->sig, i);

      f->coeff[i] += Psi*(x_i/(f->BETTA+f->norma));
    }

  //  f->DELTA = f->MEMORY_FACTOR * f->DELTA + (1 - (f->MEMORY_FACTOR)) * MIN(SQR(*err)/(f->BETTA + f->norma), f->DELTA);

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


/*
Инициализирует коэффициенты фильтра, не меня его настроек
*/
rnlms_result rnlms_clean_buff(rnlms_data_hnd rez)
{
  size_t i;

  i = 0;
  for (; i<rez->len; ++i)
    {
      rez->coeff[i] = 0.0;
    }

  rez->sig = CB_init(&(rez->coeff[rez->len]), rez->len);

  rez->err_buf = CB_init((char*)rez->sig + CB_size(rez->len), rez->ERR_BUF_LEN);

  return E_NO_ERROR;
  
}

rnlms_options rnlms_get_options(const rnlms_data_hnd mem)
{
  return (mem->opt);
}

rnlms_result rnlms_set_options(rnlms_data_hnd mem, rnlms_options new_opt)
{
  mem->opt = new_opt;
  return E_NO_ERROR;
}
