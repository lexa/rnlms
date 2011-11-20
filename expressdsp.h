#include <ialg.h>

#include "rnlms_interface.h"

//FIXME How much records in memRec ?
Int RNLMS_algAlloc(const IALG_Params *algParams, IALG_Fxns **pf, IALG_MemRec memTab[])
{
  memTab[0].size = sizeof_rnlms();
  memTab[0].alignment = 0;
  memTab[0].space = IALG_DARAM0;
  memTab[0].attrs = IALG_PERSIST;

  return 1;
}




Int RNLMS_algInit(rnlms_data_hnd handler, const IALG_MemRec *, IALG_Handle, const IALG_Params *)
{

  return (E_NO_ERROR == rnlms_result rnlms_init_struct(handler, 0.9, 0.00000001, 200, 200));
}

Int RNLMS_algFree(rnlms_data_hnd handler, IALG_MemRec memTab[])
{
  algAlloc(NULL, NULL, memTab); /* Fill the memTab struct */
	memTab[0].base = &handle;
}

typedef struct _RNLMS_XDSP 
{
	IALG_Fxns ialg;
	rnlms_result (*process) (rnlms_data_hnd rnlms_data_hnd,
			   const int16_t *x_arr,        // far abonent signal
			   const int16_t *y_arr,        // near abonent signal
			   int16_t *err_out,    // result with reduced echo
			   size_t size);

} RNLMS_XDSP ;

_RNLMS_XDSP Realisation_RNLMS_XDSP {
	&POINTER_TO_STRUCT,
	NULL,
	&RNLMS_algAlloc,
	NULL,
	NULL,
	&RNLMS_algFree,
	&RNLMS_algInit,
	NULL,
	NULL
} Realisation_RNLMS_XDSP;

asm("_POINTER_TO_STRUCT" .set "Realisation_RNLMS_XDSP");
