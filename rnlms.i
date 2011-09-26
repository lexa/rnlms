%module rlms
%{
  /* Includes the header in the wrapper code */
#include "rnlms.h"
  %}
 
/* Parse the header file to generate wrappers */
%include "rnlms.h"
 
