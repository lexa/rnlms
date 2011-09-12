#include "tester.h"


void readTwoFiles(const char *first_filename, const char *second_filename)
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


int main()
{

  readTwoFiles("g165/filtered_noise_20.dat", "g165/echo_10_128.dat"); 

  printf("exit successfull");
  return (EXIT_SUCCESS);
}
