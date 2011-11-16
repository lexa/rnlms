#ifndef RNLMS_FLOAT_H_
#define RNLMS_FLOAT_H_

#include <stdint.h> 

// Possible output description
typedef enum RNLMS_Result{
    E_NO_ERROR = 0,
      E_ERROR,
      E_BAD_MAIN_DATA,
      E_BAD_BUFF_DATA
} rnlms_result;

typedef enum RNLMS_Options{
  OPT_INHIBIT_ADAPTATION = 1,
  OPT_DISABLE_NONLINEAR_PROCESSING = 1>>1
} rnlms_options;

// Handler for one RNLMS instance
typedef struct rnlms_data* rnlms_data_hnd;

/*показывает сколько памяти портебуется для хранения фильтра длинны filter_len*/
size_t sizeof_rnlms(size_t P, size_t filter_len);
/*тоже что и sizeof_rnlms только учитывает все параметры фильтра */
//size_t sizeof_rnlms_2(float BETTA, float DELTA, float MEMORY_FACTOR, size_t filter_len);

/*инициализирует структуру филтра, по уже выделенной памяти, сама функция ничего не выделяет*/
rnlms_result rnlms_init_struct(rnlms_data_hnd mem, float ALPHA, float BETTA, size_t ERR_BUF_LEN, size_t filter_len);


// process x_arr and y_arr, puts result into err_out buffer all the buffers are served by user
rnlms_result rnlms_process(rnlms_data_hnd rnlms_data_hnd,
			   const int16_t *x_arr,        // far abonent signal
			   const int16_t *y_arr,        // near abonent signal
			   int16_t *err_out,    // result with reduced echo
			   size_t size);

/* очистка памяти алгоритма при сохранении его настроек - может */
/* пригодится, когда надо начать обработку данных заново - например, при */
/* новом звонке в телефонии. */
rnlms_result rnlms_clean_buff(rnlms_data_hnd rnlms_data_hnd);

rnlms_options rnlms_get_options(const rnlms_data_hnd mem);

rnlms_result rnlms_set_options(rnlms_data_hnd mem, rnlms_options);

#endif 
