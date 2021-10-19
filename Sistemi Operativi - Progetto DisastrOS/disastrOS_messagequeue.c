#include <assert.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define MESSAGEQUEUE_SIZE sizeof(MessageQueue)
#define MESSAGEQUEUE_MEMSIZE (sizeof(MessageQueue)+sizeof(int))
#define MESSAGEQUEUE_BUFFER_SIZE MAX_NUM_MESSAGEQUEUES*MESSAGEQUEUE_MEMSIZE

static char _messagequeues_buffer[MESSAGEQUEUE_BUFFER_SIZE];
static PoolAllocator _messagequeues_allocator;

void MessageQueue_init(){
    int result=PoolAllocator_init(& _messagequeues_allocator,
				  MESSAGEQUEUE_SIZE,
				  MAX_NUM_MESSAGEQUEUES,
				  _messagequeues_buffer,
				  MESSAGEQUEUE_BUFFER_SIZE);
    assert(! result);
}

MessageQueue* MessageQueue_alloc(int id, int type){
  MessageQueue* m=(MessageQueue*) PoolAllocator_getBlock(&_messagequeues_allocator);
  if (!m)
    return 0;
  m->list.prev=m->list.next=0;
  m->id=id;
  m->type=type;
  List_init(&m->descrittori_ptrs);
  return m;
}

int MessageQueue_free(MessageQueue* m) {
  assert(m->descrittori_ptrs.first==0);
  assert(m->descrittori_ptrs.last==0);
  return PoolAllocator_releaseBlock(&_descrittori_allocator, m);
}

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id) {
  ListItem* aux=l->first;
  while(aux){
    MessageQueue* m=(MessageQueue*)aux;
    if (m->id==id)
      return m;
    aux=aux->next;
  }
  return 0;
}

void MessageQueue_print(MessageQueue* m) {
  printf("id: %d, type:%d, pids:", m->id, m->type);
  DescrittorePtrList_print(&m->descrittori_ptrs);
}

void MessageQueueList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    MessageQueue* m=(MessageQueue*)aux;
    printf("\t");
    MessageQueue_print(m);
    if(aux->next)
      printf(",");
    printf("\n");
    aux=aux->next;
  }
  printf("}\n");
}
