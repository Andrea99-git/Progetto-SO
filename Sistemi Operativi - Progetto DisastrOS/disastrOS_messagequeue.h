#pragma once
#include <mqueue.h>
#include <sys/stat.h>
#include "linked_list.h"
#include "disastrOS_pcb.h"


typedef struct {
  ListItem list;
  int id;
  int type;
  ListHead descrittore_ptrs;
} MessageQueue;

void MessageQueue_init();

MessageQueue* MessageQueue_alloc(int id, int type);
int MessageQueue_free(MessageQueue* messagequeue);

typedef ListHead MessageQueueList;

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id);

void MessageQueueList_print(ListHead* l);
