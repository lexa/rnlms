#include "global.h"


 NUM SQR (NUM n)
{
	return (n*n);
}

NUM NUM_sqrtf (NUM n)
{
	assert (n >= 0); 
	return sqrtf(n);
}

