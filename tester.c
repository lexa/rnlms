#include "tester.h"


void testAlgo(
	      const char *far__filename)
{
			
  FILE *far__file = NULL;


  if (NULL == (far__file = fopen(far__filename, "r")))
    {
      fprintf(stderr, "can't open file\n"); return;
    }
	

  int cnt_blocks=0;
  long int total_readed_nums=0;
  while(!feof(far__file))
    {
      size_t t1;
      int16_t int16_arr1[FRAME_SIZE];
     
      t1 = fread(int16_arr1, sizeof(int16_t), FRAME_SIZE, far__file);

      total_readed_nums += t1;
      fprintf(stderr, "block #%d readed, size: %d\n", ++cnt_blocks, t1);
    }
  
  fprintf(stderr, "Total readed: %ld\n", total_readed_nums);
  fclose(far__file);
}


int main()
{

  testAlgo("g165/filtered_noise_20.dat"); 

  printf("exit successfull");
  return (EXIT_SUCCESS);
}
