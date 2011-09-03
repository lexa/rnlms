#include "global.h"


NUM sqr (NUM n)
{
	return (n*n);
}

NUM my_sqrtf (NUM n)
{
	assert (n >= 0);
	return sqrtf(n);
}

