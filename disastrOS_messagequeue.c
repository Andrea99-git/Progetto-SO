#include <assert.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define MESSAGEQUEUE_SIZE sizeof(MessageQueue)
#define MESSAGEQUEUE_MEMSIZE (sizeof(MessageQueue)+sizeof(int))
#define MESSAGEQUEUE_BUFFER_SIZE MAX_NUM_MESSAGEQUEUES*MESSAGEQUEUE_MEMSIZE

#define MESSAGE_SIZE sizeof(Message)
#define MESSAGE_MEMSIZE (sizeof(Message)+sizeof(int))
#define MESSAGE_BUFFER_SIZE MAX_NUM_MESSAGES*MESSAGE_MEMSIZE

static char _messagequeues_buffer[MESSAGEQUEUE_BUFFER_SIZE];
static PoolAllocator _messagequeues_allocator;

static char _message_buffer[MESSAGE_BUFFER_SIZE];
static PoolAllocator _message_allocator;

void MessageQueue_init(){
    int result=PoolAllocator_init(& _messagequeues_allocator,
				  MESSAGEQUEUE_SIZE,
				  MAX_NUM_MESSAGEQUEUES,
				  _messagequeues_buffer,
				  MESSAGEQUEUE_BUFFER_SIZE);
    assert(! result);

   result=PoolAllocator_init(& _message_allocator,
        MESSAGE_SIZE,
        MAX_NUM_MESSAGES,
        _message_buffer,
        MESSAGE_BUFFER_SIZE);
   assert(! result);

}

MessageQueue* MessageQueue_alloc(int id, int type, int msg_num, int msg_size, int max_msg){
  MessageQueue* m=(MessageQueue*) PoolAllocator_getBlock(&_messagequeues_allocator);
  if (!m)
    return 0;
  m->list.prev=m->list.next=0;
  m->id=id;
  m->type=type;
  m->msg_num = 0;
  m->msg_size=msg_size;
  m->max_msg=max_msg;
  List_init(&m->descrittori_ptrs);
  List_init(&m->messages);
  return m;
}

int MessageQueue_free(MessageQueue* m) {
  assert(m->descrittori_ptrs.first==0);
  assert(m->descrittori_ptrs.last==0);
  assert(m->messages.first==0);
  assert(m->messages.last==0);
  return PoolAllocator_releaseBlock(&_messagequeues_allocator, m);
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

//Messages

Message* Message_alloc(int id) {
  Message* msg=(Message*) PoolAllocator_getBlock(&_message_allocator);
  if (!msg)
    return 0;
  msg->list.prev=msg->list.next=0;
  msg->id=id;
  return msg;
}

int Message_free(Message* msg){
  return PoolAllocator_releaseBlock(&_message_allocator, msg);
}

Message* Message_byId(ListHead* messages, int id){
  ListItem* aux=messages->first;
  while (aux) {
    Message* message=(Message*) aux;
    if (message->id==id){
      return message;
      aux=aux->next;
    }
  }
  return 0;
}

void MessageList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    Message* m=(Message*)aux;
    printf("(id: %d, message:%s)",
     m->id,
     m->message);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}

