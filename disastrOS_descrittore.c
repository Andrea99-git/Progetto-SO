#include <assert.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "linked_list.h"
#include "disastrOS_descrittore.h"
#include "pool_allocator.h"
#include "disastrOS_constants.h"

#define DESCRITTORE_SIZE sizeof(Descrittore)
#define DESCRITTORE_MEMSIZE (sizeof(Descrittore)+sizeof(int))
#define MAX_NUM_DESCRITTORI (MAX_NUM_DESCRITTORI_PER_PROCESS*MAX_NUM_PROCESSES)
#define DESCRITTORE_BUFFER_SIZE MAX_NUM_DESCRITTORI*DESCRITTORE_MEMSIZE

#define DESCRITTOREPTR_SIZE sizeof(DescrittorePtr)
#define DESCRITTOREPTR_MEMSIZE (sizeof(DescrittorePtr)+sizeof(int))
#define DESCRITTOREPTR_BUFFER_SIZE MAX_NUM_DESCRITTORI*DESCRITTOREPTR_MEMSIZE

static char _descrittore_buffer[DESCRITTORE_BUFFER_SIZE];
static PoolAllocator _descrittore_allocator;

static char _descrittore_ptr_buffer[DESCRITTOREPTR_BUFFER_SIZE];
static PoolAllocator _descrittore_ptr_allocator;

void Descrittore_init(){
  int result=PoolAllocator_init(& _descrittore_allocator,
				DESCRITTORE_SIZE,
				MAX_NUM_PROCESSES,
				_descrittore_buffer,
				DESCRITTORE_BUFFER_SIZE);
  assert(! result);

  result=PoolAllocator_init(& _descrittore_ptr_allocator,
			    DESCRITTOREPTR_SIZE,
			    MAX_NUM_PROCESSES,
			    _descrittore_ptr_buffer,
			    DESCRITTOREPTR_BUFFER_SIZE);
  assert(! result);
}

Descrittore* Descrittore_alloc(int mqd, MessageQueue* mq, PCB* pcb) {
  Descrittore* d=(Descrittore*)PoolAllocator_getBlock(&_descrittore_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->mqd=mqd;
  d->messagequeue=mq;
  d->pcb=pcb;
  return d;
}

int Descrittore_free(Descrittore* d) {
  return PoolAllocator_releaseBlock(&_descrittore_allocator, d);
}

Descrittore*  DescrittoreList_byMqd(ListHead* l, int mqd){
  ListItem* aux=l->first;
  while(aux){
    Descrittore* d=(Descrittore*)aux;
    if (d->mqd==mqd)
      return d;
    aux=aux->next;
  }
  return 0;
}

DescrittorePtr* DescrittorePtr_alloc(Descrittore* descrittore) {
  DescrittorePtr* d=PoolAllocator_getBlock(&_descrittore_ptr_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->descrittore=descrittore;
  return d;
}

int DescrittorePtr_free(DescrittorePtr* d){
  return PoolAllocator_releaseBlock(&_descrittore_ptr_allocator, d);
}

void DescrittoreList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    Descrittore* d=(Descrittore*)aux;
    printf("(des: %d, rid:%d\n)",
     d->mqd,
	   d->messagequeue->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}


void DescrittorePtrList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    DescrittorePtr* d=(DescrittorePtr*)aux;
    printf("(pid: %d,mqd: %d, rid:%d)",
	   d->descrittore->pcb->pid,
     d->descrittore->mqd,
	   d->descrittore->messagequeue->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}
