#include "tester.h"


/* typedef NUM (*FunctionOfTwoArgs)(void* pFilter, NUM, NUM, NUM*, NUM*); */

/* void testBlock(FunctionOfTwoArgs func, void  *filterStruct, const NUM *far_, const NUM *near_, NUM *err, NUM* output, size_t arrayLen) */
/* { */
/*   size_t i=0; */
/*   for (; i< arrayLen; ++i) */
/*     { */
/*       func(filterStruct, far_[i], near_[i], &err[i], &output[i]); */
/*     } */
/* } */

void testAlgo(rnlms_data_hnd filterStruct,
	      const char *far__filename,
	      const char *near__filename,
	      const char *err_filename,
	      const char *output_filename)
{
			
  FILE *far__file = NULL,
    *near__file = NULL,
    *err_file = NULL;


  if (NULL == (far__file = fopen(far__filename, "rb")))
    {
      fprintf(stderr, "can't open file\n"); return;
    }
  
  if (NULL == (near__file = fopen(near__filename, "rb")))
    {
      fclose(far__file);
      fprintf(stderr, "can't open file\n"); return;
    }
  
  if (NULL == (err_file = fopen(err_filename, "wb")))
    {
      fclose(far__file);
      fclose(near__file);
      fprintf(stderr, "can't open file\n"); return;
    }
   /* if (NULL == (output_file = fopen(output_filename, "wb"))) */
   /*   { */
   /*     fclose(far__file); */
   /*     fclose(near__file); */
   /*     fclose(err_file); */
   /*     fprintf(stderr, "can't open file\n"); return; */
   /*   } */
	

  while(!feof(far__file) && !feof(near__file))
    {
      /*      int i;*/
      size_t  readedNums, t1, t2;
      int16_t far_arr[FRAME_SIZE];
      int16_t near_arr[FRAME_SIZE];
      int16_t err_arr[FRAME_SIZE];
	  
      /*читает один блок*/

      if (FRAME_SIZE != (t1 = fread(far_arr, sizeof(int16_t), FRAME_SIZE, far__file)))
      	{
      	  //fprintf(stderr, "warning, readed less then FRAME_SIZE(from far__file)");
	  break;
      	}
      
      if (FRAME_SIZE != (t2 = fread(near_arr, sizeof(int16_t), FRAME_SIZE, near__file)))
      	{
      	  //fprintf(stderr, "warning, readed less then FRAME_SIZE(from near__file)");
	  break;
	  
      	}
      
      readedNums = MIN (t1,t2);
      
      //      printf("one block readed\n");
      rnlms_process(filterStruct, far_arr, near_arr, err_arr, readedNums);
      //printf("one block processed\n");
	  
      /* /\*сохраняет обработанные данные*\/ */

      if (fwrite(err_arr, sizeof(int16_t), readedNums, err_file) != readedNums)
      	{
      	  fprintf(stderr, "can't write to file\n"); return;
      	}
    }
  
  
  fclose(far__file);
  fclose(near__file);
  fclose(err_file);
}





int main()
{
  rnlms_data_hnd filterStruct = malloc(sizeof_rnlms(FILTER_LEN)); 
  assert(NULL != filterStruct);

  rnlms_init_struct(filterStruct, 0.00000000000000001, .01f, 0.999, FILTER_LEN);

  //  rnlms_set_options(filterStruct, OPT_INHIBIT_ADAPTATION);
  
  testAlgo(filterStruct,	       \
  	   "g165/echo_10_128.dat",	       \
  	   "g165/filtered_noise_10.dat",       \
  	   "error.dat",			       \
  	   "output.dat"
  	   );

  /* testAlgo(rnlms_func, filterStruct,	       \ */
  /* 	   "torvalds-says-linux.pcm",       \ */
  /* 	   "torvalds-says-linux_echo.pcm",	       \ */
  /* 	   "error.dat",			       \ */
  /* 	   "output.dat" */
  /* 	   ); */

   /* testAlgo(rnlms_func, filterStruct,	       \ */
  /* 	   "noise_two_levels.pcm",       \ */
  /* 	   "noise_two_levels_echo.pcm",	       \ */
  /* 	   "error.dat",			       \ */
  /* 	   "output.dat" */
  /* 	   ); */

  free(filterStruct); 
  printf("exit successfull");
  return (EXIT_SUCCESS);
}
