#ifndef _H_MEDIAN_
#define _H_MEDIAN_

//typedef struct _Mediator Mediator;

typedef struct Mediator_t
{
  NUM* data;  //circular queue of values
  int*  pos;   //index into `heap` for each value
  int*  heap;  //max/median/min heap holding indexes into `data`.
  int   N;     //allocated size.
  int   idx;   //position in circular queue
  int   ct;    //count of items in queue
} Mediator;


Mediator* MediatorNew(void* mem, size_t nItems);
void MediatorInsert(Mediator* m, NUM v);
NUM MediatorMedian(const Mediator* m);
size_t Mediator_sizeof(size_t nItems);

#endif
