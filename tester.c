#include "tester.h"


void testAlgo(
	      const char *far__filename)
{
			
  FILE *far__file = NULL;


  if (NULL == (far__file = fopen(far__filename, "r")))
    {
      fprintf(stderr, "can't open file\n"); return;
    }
	

  while(!feof(far__file))
    {
      size_t t1;
      int16_t int16_arr1[FRAME_SIZE];
     
      t1 = fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file);

 
      fprintf(stderr, "one block updated %u\n", t1);
	  
    }
	
	
  fclose(far__file);
}


int main()
{

  testAlgo("g165/filtered_noise_20.dat"); 

  printf("exit successfull");
  return (EXIT_SUCCESS);
}
