#include "tester.h"


typedef NUM (*FunctionOfTwoArgs)(void* pFilter, NUM, NUM, NUM*, NUM*);

void testBlock(FunctionOfTwoArgs func, void  *filterStruct, const NUM *far_, const NUM *near_, NUM *err, NUM* output, size_t arrayLen)
{
  size_t i=0;
  for (; i< arrayLen; ++i)
    {
      func(filterStruct, far_[i], near_[i], &err[i], &output[i]);
    }
}

void testAlgo(FunctionOfTwoArgs func, void* filterStruct,
	      const char *far__filename,
	      const char *near__filename,
	      const char *err_filename,
	      const char *output_filename)
{
  /*	void *filterStruct = initializer();*/
  NUM far_ [FRAME_SIZE];
  NUM near_ [FRAME_SIZE];
  NUM err [FRAME_SIZE];
  NUM output [FRAME_SIZE];

  /* int16_t f_, n_; */
  /* size_t writedNums; */
			
  FILE *far__file = NULL,
    *near__file = NULL,
    *err_file = NULL,
    *output_file = NULL;



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
  if (NULL == (output_file = fopen(output_filename, "wb")))
    {
      fclose(far__file);
      fclose(near__file);
      fclose(err_file);
      fprintf(stderr, "can't open file\n"); return;
    }
	

  while(!feof(far__file) && !feof(near__file))
    {
      /*      int i;*/
      size_t  readedNums, t1, t2;
      int16_t int16_arr1[FRAME_SIZE];
      int16_t int16_arr2[FRAME_SIZE];
	  
      /* memset (int16_arr1, 0, FRAME_SIZE * 2); */
      /* memset (int16_arr2, 0, FRAME_SIZE *  2); */
      
      
      /*читает один блок*/

      if (FRAME_SIZE != (t1 = fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file)))
      	{
      	  fprintf(stderr, "warning, readed less then FRAME_SIZE(from far__file)");
      	}
      
      if (FRAME_SIZE != (t2 = fread(int16_arr2, sizeof(int16_t), FRAME_SIZE, near__file)))
      	{
      	  fprintf(stderr, "warning, readed less then FRAME_SIZE(from near__file)");
      	}
      
      /* for (i=0; i<FRAME_SIZE; i++) */
      /* 	{ */
      /* 	  int tmp1, tmp2; */
      /* 	  fscanf(far__file, "%d", &tmp1); */
      /* 	  fscanf(near__file, "%d", &tmp2); */
      /* 	  int16_arr1[i] = tmp1; */
      /* 	  int16_arr2[i] = tmp2; */
      /* 	  //	  printf("%d\n", tmp1); */
      /* 	} */

      /* t2=t1=FRAME_SIZE; */

      readedNums = MIN_size_t (t1,t2);
      
      convert_from_int16_to_NUM(int16_arr1, far_, readedNums);
      convert_from_int16_to_NUM(int16_arr2, near_, readedNums);
	  
      /* прогоняет данные через тестовый алгоритм */
      testBlock(func, filterStruct, far_, near_, err, output, readedNums);
	  
      /*сохраняет обработанные данные*/
      convert_from_NUM_to_int16(err, int16_arr1, readedNums);
      convert_from_NUM_to_int16(output, int16_arr2, readedNums);

	  /* for (i=0; i<readedNums; i++) */
	  /* { */
	  /* 	fprintf(err_file, "%d\n", int16_arr1[i]); */
	  /* } */

      if (fwrite(int16_arr1, sizeof(int16_t), readedNums, err_file) != readedNums)
      	{
      	  fprintf(stderr, "can't write to file\n"); return;
      	}

      if (fwrite(int16_arr2, sizeof(int16_t), readedNums, output_file) != readedNums)
      	{
      	  fprintf(stderr, "can't write to file\n"); return;
      	}

      /*      fprintf(stderr, "one block updated %u\n", readedNums);*/
	  
    }
  
  
  fclose(far__file);
  fclose(near__file);
  fclose(err_file);
  fclose(output_file);
}





int main()
{
  void *filterStruct = malloc(rlms_sizeOfRequiredMemory(FILTER_LEN)); 
  assert(NULL != filterStruct);

  
  rlms_init(filterStruct, 100, 0.1f, 0.999f, FILTER_LEN);
  
  
  testAlgo(rlms_func, filterStruct,	       \
	   "g165/filtered_noise_10.dat",       \
	   "g165/echo_10_128.dat",	       \
	   "error.dat",			       \
	   "output.dat"
	   ); 

  free(filterStruct); 
  printf("exit successfull");
  return (EXIT_SUCCESS);
}
