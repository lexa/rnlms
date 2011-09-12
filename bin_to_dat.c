/* Программа конвертирует двоичные файлы с 2х байтными числами в формат который понимает probe points */

#include <stdio.h>
#include <stdint.h>

#define ABS(x) ((x>=0)?x:(-x))


int main(int argc, char **argv)
{
  FILE *in;
  FILE *out;
  
  in = fopen(argv[1], "r");
  out = fopen(argv[2], "w");

  //  fprintf(out, "1651 1 0 0 0\n");
  while (! (feof(in)))
    {
      int16_t tmp;
      fread(&tmp, sizeof(int16_t), 1, in);
      //      fprintf(out, "0x%0.8x\n", tmp);
      fprintf(out, "%d\n", tmp);
    }
  fclose(in);
  fclose(out);
  return 0;
}

/* http://www.dsprelated.com/groups/code-comp/show/706.php */

 
/* From CCS user manual: */
 

/* The header information for data files uses the following syntax: */

/* MagicNumber  Format StartingAddress PageNum Length */
/* MagicNumber Fixed at 1651. */
/* Format A number from 1 to 4, indicating the format of the samples in the file. This number represents a data format: hexadecimal, integer, long, or float. */
/* StartingAddress The starting address of the block that was saved. */
/* PageNum The page number the block was taken from. */
/* Length The number of samples in the block. */

/* All header values are assumed to be TI-style hexadecimal values. */
/* The following is an example of a Code Composer Studio data file: */

/* 1651 1 800 1 10 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */
/* 0x0000 */


/* Note that CCS expects 5 digit numbers of information to read in 4-digit values. While  the data is known to be hexadecimal, CCS expects the  first digit to be a zero. CCS does this so that hex numbers beginning  with letters (ie. F800) are not misread as labels. For example: when reading data in hexadecimal format from a data file, the first digit may be truncated. Look at this input data:  */

/* 0022 */
/* 0022 */
/* 0033 */
/* 8AC  */
/* FC94 */
/* 13 */
/* 895 */
/* AB9 ... */

/* When reading this data, CCS will actually read  it as: */

/* 0022 */
/* 0033 */
/* 08AC */
/* 0C94 */
/* 0003 */
/* 0095 */
/* 00B9 ... */

/* But if you  test Code Composer Studio with five-digit data input, (by adding zeroes at the beginning where necessary), the output will be  consistent with the input: */

/* 00022 */
/* 00033 */
/* 018AC */
/* 0FC94 */
/* 00013 */
/* 00895 */
/* 00AB9 */

/* Note: Header values specify only the default address and length. When you use the File®Data */
/* ®Load command to load a file into memory, the Code Composer Studio debugger gives you a chance to override these values. When using the Code Composer Studio data file format with file I/O capabilities, any information you enter in the File I/O dialog box (Address and Length) automatically overrides the Code Composer Studio data file header information. You do not need to set the header information for each file as long as the header includes the following value: 1651 1 0 0 0.  */
