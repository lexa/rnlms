#include "rnlms.h"

#define LEVEL_MEASUREMENT_FILTER_LEN 101

/* const NUM level_measurement_filter_coeff[LEVEL_MEASUREMENT_FILTER_LEN]={0.0000f, 0.0006f, 0.0005f, 0.0004f, 0.0011f, -0.0000f, 0.0015f, -0.0003f, 0.0012f, -0.0002f, 0.0000f, 0.0002f, -0.0020f, 0.0005f, -0.0040f, 0.0000f, -0.0047f, -0.0019f, -0.0033f, -0.0047f, -0.0000f, -0.0068f, 0.0036f, -0.0057f, 0.0054f, 0.0000f, 0.0044f, 0.0095f, 0.0017f, 0.0188f, 0.0000f, 0.0225f, 0.0024f, 0.0163f, 0.0092f, 0.0000f, 0.0164f, -0.0210f, 0.0161f, -0.0375f, 0.0000f, -0.0406f, -0.0357f, -0.0267f, -0.0871f, -0.0000f, -0.1420f, 0.0289f, -0.1843f, 0.0475f, 0.8006f, 0.0475f, -0.1843f, 0.0289f, -0.1420f, -0.0000f, -0.0871f, -0.0267f, -0.0357f, -0.0406f, 0.0000f, -0.0375f, 0.0161f, -0.0210f, 0.0164f, 0.0000f, 0.0092f, 0.0163f, 0.0024f, 0.0225f, 0.0000f, 0.0188f, 0.0017f, 0.0095f, 0.0044f, 0.0000f, 0.0054f, -0.0057f, 0.0036f, -0.0068f, -0.0000f, -0.0047f, -0.0033f, -0.0019f, -0.0047f, 0.0000f, -0.0040f, 0.0005f, -0.0020f, 0.0002f, 0.0000f, -0.0002f, 0.0012f, -0.0003f, 0.0015f, -0.0000f, 0.0011f, 0.0004f, 0.0005f, 0.0006f, 0.0000f}; */

static  NUM level_measurement_filter_coeff[LEVEL_MEASUREMENT_FILTER_LEN] = {0.0f};

size_t sizeof_rnlms(size_t P, size_t filter_len)
{
  return sizeof(struct rnlms_data) + \
    (sizeof(NUM)*filter_len) + \
    (CB_size(filter_len)) + 
    (CB_size(LEVEL_MEASUREMENT_FILTER_LEN)) + 
    (Mediator_sizeof(P));
}

rnlms_result rnlms_init_by_struct(rnlms_data_hnd mem, RNLMS_params p)
{
  memcpy(mem, &p, sizeof(p));
  return rnlms_clean_buff(mem);
}

rnlms_result rnlms_init(rnlms_data_hnd mem, NUM ALPHA, NUM BETTA, size_t ERR_BUF_LEN, size_t filter_len, NUM NLP_thresold)
{
  struct rnlms_data* rez = mem;
  
  for (int i=0; i< LEVEL_MEASUREMENT_FILTER_LEN; i++)
    level_measurement_filter_coeff[i] = 1.0f;
  
 
  rez->param.len = filter_len;
  rez->param.ALPHA = ALPHA;
  rez->param.BETTA = BETTA;
  rez->param.ERR_BUF_LEN = ERR_BUF_LEN;
  rez->param.NLP_thresold=NLP_thresold;
  rez->norma = 0.0; 
  rez->opt = 0;
  rez->coeff = (NUM*)(rez + 1);

  return rnlms_clean_buff(mem);
}

/* int */
/* compare_NUM (const void * a, const void * b) */
/* { */
/*   return ( *(NUM*)a - *(NUM*)b ); */
/* } */



/*
Обрабатывает один отсчёт сигнала:
1) расчитывает выход фильтра
2) адаптирует коэффициенты фильтра
*/
NUM rnlms_func(rnlms_data_hnd f, NUM far_, NUM near_, NUM *err, NUM *output)
{
  //  size_t i;
  NUM Psi;
  NUM mediana;
  
  
  f->norma += SQR(near_) - SQR(CB_get_first_elem(f->sig)) ;//  f->norma == X*X' 

  CB_push_elem(f->sig, near_);

  *output = convolution_CB_and_vector(f->sig, f->coeff); //выход фильтра 
  *err = far_ - *output; 

  if (f->opt & OPT_INHIBIT_ADAPTATION)
    return *err;


  //CB_push_elem(f->err_buf, *err);
  MediatorInsert(f->err_buf, *err);
  


  //  NUM mediana = CB_mediana(f->err_buf);
  mediana = MediatorMedian(f->err_buf);

  //  {
  //    NUM tmp[3];
  //    memcpy(tmp, f->err_buf->data, sizeof(NUM)*f->param.ERR_BUF_LEN);
  //    qsort(tmp, f->param.ERR_BUF_LEN, sizeof(NUM), compare_NUM);
  //    fprintf(stderr, "%f %f\n", *err,  mediana);
  //  }


  if (3 * NUM_abs(mediana) > *err) //FIXME почему 3 ???
    Psi = *err * (f->param.ALPHA);
  else
    Psi = 0.5f * mediana * (f->param.ALPHA) * SIGN(*err); 


//  NUM tmp = Psi/(f->param.BETTA+f->norma);

  CB_multiple_and_add(f->sig, Psi/(f->param.BETTA+f->norma), &(f->coeff));
  
  /* for (i =0; i<f->param.len; ++i) */
  /*   { */
  /*     NUM x_i = CB_get_elem(f->sig, i); */
  /*     f->coeff[i] += Psi*(x_i/(f->param.BETTA+f->norma)); */
  /*   } */

  //  f->DELTA = f->MEMORY_FACTOR * f->DELTA + (1 - (f->MEMORY_FACTOR)) * MIN(SQR(*err)/(f->BETTA + f->norma), f->DELTA);

  if (!(f->opt & OPT_DISABLE_NONLINEAR_PROCESSING))
    {
      //      CB_push_elem(f->err_signal, SQR(*err/65535));
      CB_push_elem(f->err_signal, SQR(*err));
      double pwr = convolution_CB_and_vector(f->err_signal, level_measurement_filter_coeff);
      double level = (+10 * log10(pwr/LEVEL_MEASUREMENT_FILTER_LEN) - 83.11858286715683);
      fprintf(stderr, "level : %g\n", level);
      if (level < f->param.NLP_thresold)
      	*err=0;
    }


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
  for (; i<rez->param.len; ++i)
    {
      rez->coeff[i] = 0.0;
    }

  rez->sig = CB_init(&(rez->coeff[rez->param.len]), rez->param.len);
  rez->err_signal = CB_init((char*)rez->sig + CB_size(rez->param.len), LEVEL_MEASUREMENT_FILTER_LEN);

  rez->err_buf = MediatorNew((char*)rez->err_signal + CB_size(LEVEL_MEASUREMENT_FILTER_LEN), rez->param.ERR_BUF_LEN);
//CB_init((char*)rez->sig + CB_size(rez->len), rez->ERR_BUF_LEN);

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

void rnlms_show_debug(rnlms_data_hnd mem)
{
  for (unsigned int i=0; i<mem->param.len; i++)
    {
      printf("%g ", mem->coeff[i]);
    }
  printf("\n");
}
