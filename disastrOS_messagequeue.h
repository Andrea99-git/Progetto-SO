#pragma once
#include <mqueue.h>
#include <sys/stat.h>
#include "linked_list.h"
#include "disastrOS_pcb.h"

//devo introdurre un valore int nummsg (che indica il numero di messaggi totale) ed il buffer (che contiene i messaggi)

//il buffer penso vada allocato come buffer[maxmsg][msgsize] e per scrivere al suo interno dovro' usare
//strcpy(buffer[0], "blah");

typedef struct{
  ListItem list;
  int id;
  char* message;
} Message;

typedef struct {
  ListItem list;
  int id;
  int type;
  int msg_num;  //indicates the number of messages in a MessageQueue
  int msg_size; 	//max size of a msg
  int max_msg;	// max number of messages in a mq
  ListHead descrittori_ptrs;
  ListHead messages;
} MessageQueue;

void MessageQueue_init();

MessageQueue* MessageQueue_alloc(int id, int type, int msg_num, int msg_size, int max_msg);
int MessageQueue_free(MessageQueue* messagequeue);

typedef ListHead MessageQueueList;

MessageQueue* MessageQueueList_byId(MessageQueueList* l, int id);

void MessageQueueList_print(ListHead* l);

//Messages

Message* Message_alloc(int id);
int Message_free(Message* msg);
Message* Message_byId(ListHead* messages, int id);
void MessageList_print(ListHead* l);