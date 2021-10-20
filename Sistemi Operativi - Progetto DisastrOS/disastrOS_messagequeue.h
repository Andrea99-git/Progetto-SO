#pragma once
#include <mqueue.h>
#include <sys/stat.h>
#include "linked_list.h"
#include "disastrOS_pcb.h"

//devo introdurre un valore int nummsg (che indica il numero di messaggi totale) ed il buffer (che contiene i messaggi)

//il buffer penso vada allocato come buffer[maxmsg][msgsize] e per scrivere al suo interno dovro' usare
//strcpy(buffer[0], "blah");

typedef struct {
  ListItem list;
  int id;
  int type;
  int msgsize; 	//max size of a msg
  int maxmsg;	// max number of messages in a mq
  ListHead descrittore_ptrs;
} MessageQueue;

void MessageQueue_init();

MessageQueue* MessageQueue_alloc(int id, int type, int msgsize; int maxmsg);
int MessageQueue_free(MessageQueue* messagequeue);

typedef ListHead MessageQueueList;

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id);

void MessageQueueList_print(ListHead* l);
