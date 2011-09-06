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

  
  
  if (NULL == (far__file = fopen(far__filename, "r")))
    {
      fprintf(stderr, "can't open file\n"); return;
    }
  
  if (NULL == (near__file = fopen(near__filename, "r")))
    {
      fclose(far__file);
      fprintf(stderr, "can't open file\n"); return;
    }
  
  if (NULL == (err_file = fopen(err_filename, "w")))
    {
      fclose(far__file);
      fclose(near__file);
      fprintf(stderr, "can't open file\n"); return;
    }
  if (NULL == (output_file = fopen(output_filename, "w")))
    {
      fclose(far__file);
      fclose(near__file);
      fclose(err_file);
      fprintf(stderr, "can't open file\n"); return;
    }
	

  while(!feof(far__file) && !feof(near__file))
    {

      size_t  readedNums;
      int16_t int16_arr1[FRAME_SIZE];
      int16_t int16_arr2[FRAME_SIZE];
	  

      /*читает один блок*/
      readedNums = MIN_size_t(\
      			   fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file), \
      			   fread(int16_arr2, sizeof(int16_t), FRAME_SIZE, near__file));

      convert_from_int16_to_NUM(int16_arr1, far_, readedNums);
      convert_from_int16_to_NUM(int16_arr2, near_, readedNums);
	  
      /* прогоняет данные через тестовый алгоритм */
      testBlock(func, filterStruct, far_, near_, err, output, readedNums);
	  
      /*сохраняет обработанные данные*/

      convert_from_NUM_to_int16(err, int16_arr1, readedNums);
      convert_from_NUM_to_int16(output, int16_arr2, readedNums);

      fwrite(int16_arr1, sizeof(int16_t), readedNums, err_file);
      fwrite(int16_arr2, sizeof(int16_t), readedNums, output_file);
    }
	
	
  fclose(far__file);
  fclose(near__file);
  fclose(err_file);
  fclose(output_file); 
}

char filterStruct[10000];

int main()
{
  /* void *filterStruct = malloc(rlms_sizeOfRequiredMemory(FILTER_LEN)); */
  /* fprintf(stderr, "required mem size: %d", rlms_sizeOfRequiredMemory(FILTER_LEN)); */
  assert(rlms_sizeOfRequiredMemory(FILTER_LEN) < sizeof (filterStruct));

  
  rlms_init(filterStruct, 100, 0.1f, 0.999f, FILTER_LEN);
  
  
  testAlgo(rlms_func, filterStruct,	       \
	   "g165/filtered_noise_10.dat",       \
	   "g165/echo_10_128.dat",	       \
	   "error.dat",			       \
	   "output.dat"
	   ); 

  /*free(filterStruct); */
  printf("extint successfull");
  return (EXIT_SUCCESS);
}
 
 
