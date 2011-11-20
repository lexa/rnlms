#include <ialg.h>

#include "rnlms_interface.h"

Int RNLMS_Alloc(const IALG_Params *algParams, IALG_Fxns **pf, IALG_MemRec memTab[])
{
  memTab[0].size = sizeof_rnlms();
  memTab[0].alignment = 0;
  memTab[0].space = IALG_DARAM0;
  memTab[0].attrs = IALG_PERSIST;
  
  return 1;
}
