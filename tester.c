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

/* void testAlgo(FunctionOfTwoArgs func, void* filterStruct,  */
/* 	      const char *far__filename, */
/* 	      const char *near__filename, */
/* 	      const char *err_filename,  */
/* 	      const char *output_filename) */
/* { */
/*   /\*	void *filterStruct = initializer();*\/ */
/*   NUM far_ [FRAME_SIZE]; */
/*   NUM near_ [FRAME_SIZE]; */
/*   NUM err [FRAME_SIZE]; */
/*   NUM output [FRAME_SIZE]; */

/*   /\* int16_t f_, n_; *\/ */
/*   /\* size_t writedNums; *\/ */
			
/*   FILE *far__file = NULL, */
/*     *near__file = NULL, */
/*     *err_file = NULL, */
/*     *output_file = NULL; */



/*   if (NULL == (far__file = fopen(far__filename, "rb"))) */
/*     { */
/*       fprintf(stderr, "can't open file\n"); return; */
/*     } */
  
/*   if (NULL == (near__file = fopen(near__filename, "rb"))) */
/*     { */
/*       fclose(far__file); */
/*       fprintf(stderr, "can't open file\n"); return; */
/*     } */
  
/*   if (NULL == (err_file = fopen(err_filename, "w"))) */
/*     { */
/*       fclose(far__file); */
/*       fclose(near__file); */
/*       fprintf(stderr, "can't open file\n"); return; */
/*     } */
/*   if (NULL == (output_file = fopen(output_filename, "w"))) */
/*     { */
/*       fclose(far__file); */
/*       fclose(near__file); */
/*       fclose(err_file); */
/*       fprintf(stderr, "can't open file\n"); return; */
/*     } */
	

/*   while(!feof(far__file) && !feof(near__file)) */
/*     { */
/* 		int i; */
/*       size_t  readedNums=255, t1=255, t2=255; */
/*       int16_t int16_arr1[FRAME_SIZE]={255}; */
/*       int16_t int16_arr2[FRAME_SIZE]={255}; */
	  
      

/*       if (FRAME_SIZE != (t1 = fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file))) */
/* 	  { */
/* 	  fprintf(stderr, "warning, readed less then FRAME_SIZE(from far__file)"); */
/* 	  } */

/* 	  if (FRAME_SIZE != (t2 = fread(int16_arr2, sizeof(int16_t), FRAME_SIZE, near__file))) */
/* 	  { */

/* 	  fprintf(stderr, "warning, readed less then FRAME_SIZE(from near__file) %d", errno); */
/* 	  } */

      
/*       /\*читает один блок*\/ */
/*       /\*      readedNums = MIN_size_t(					\ */
/* 	      fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file), \ */
/* 	      fread(int16_arr2, sizeof(int16_t), FRAME_SIZE, near__file));  *\/ */
      
/*       readedNums = MIN_size_t (t1,t2); */
      
/*       convert_from_int16_to_NUM(int16_arr1, far_, readedNums); */
/*       convert_from_int16_to_NUM(int16_arr2, near_, readedNums); */
	  
/*       /\* прогоняет данные через тестовый алгоритм *\/ */
/*       testBlock(func, filterStruct, far_, near_, err, output, readedNums); */
	  
/*       /\*сохраняет обработанные данные*\/ */

/*       convert_from_NUM_to_int16(err, int16_arr1, readedNums); */
/*       convert_from_NUM_to_int16(output, int16_arr2, readedNums); */

/* 	  for (i=0; i<readedNums; i++) */
/* 	  { */
/* 	  	fprintf(stderr, "%d\n", int16_arr1[i]); */
/* 	  } */

/* /\*      if (fwrite(int16_arr1, sizeof(int16_t), readedNums, err_file) != readedNums) */
/* 	  { */
/*       	fprintf(stderr, "can't write to file\n"); return; */
/* 		} */

/*       if (fwrite(int16_arr2, sizeof(int16_t), readedNums, output_file) != readedNums) */
/* 	  { */
/*       	fprintf(stderr, "can't write to file\n"); return; */
/* 		} *\/ */

/*       fprintf(stderr, "one block updated %u\n", readedNums); */
	  
/*     } */
	
	
/*   fclose(far__file); */
/*   fclose(near__file); */
/*   fclose(err_file); */
/*   fclose(output_file);  */
/* } */


void testAlgo(FunctionOfTwoArgs func, void* filterStruct,
	      const char *first_filename,
	      const char *second_filename,
	      const char *err_filename,
	      const char *output_filename)
{

  FILE *first_file = NULL;
  FILE *second_file = NULL;
  size_t readedBlocks = 0;

  size_t readed_from_first = 0;
  size_t readed_from_second = 0;



  first_file = fopen(first_filename, "rb");

  second_file = fopen(second_filename, "rb");


  while(!feof(first_file) && !feof(second_file))
    {
      size_t t1, t2;
      int16_t int16_arr1[FRAME_SIZE];
      int16_t int16_arr2[FRAME_SIZE];
     
      t1 = fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, first_file);
      t2 = fread(int16_arr2, sizeof(int16_t), FRAME_SIZE, second_file);

      readed_from_first += t1;
      readed_from_second += t2;
      fprintf(stderr, "block #%d, readed %d bytes from first file\nreaded %d bytes from second file\n", ++readedBlocks, t1*sizeof(int16_t), t2*sizeof(int16_t));
    }
  
  fclose(first_file);
  fclose(second_file);
}


//char filterStruct[1000];

int main()
{
  void *filterStruct = malloc(rlms_sizeOfRequiredMemory(FILTER_LEN)); 
  assert(NULL != filterStruct);
  /* fprintf(stderr, "required mem size: %d", rlms_sizeOfRequiredMemory(FILTER_LEN)); */
  //assert(rlms_sizeOfRequiredMemory(FILTER_LEN) < sizeof (filterStruct));

  
  rlms_init(filterStruct, 100, 0.1f, 0.999f, FILTER_LEN);
  
  
  testAlgo(rlms_func, filterStruct,	       \
	   "g165/filtered_noise_20.dat",       \
	   "g165/echo_10_128.dat",	       \
	   "error.dat",			       \
	   "output.dat"
	   ); 

  free(filterStruct); 
  printf("exit successfull");
  return (EXIT_SUCCESS);
}
