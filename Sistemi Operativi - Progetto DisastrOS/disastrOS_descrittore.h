#pragma once
#include <sys/time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "disastrOS_messagequeue.h"


struct DescrittorePtr;

typedef struct Descrittore{
  ListItem list;
  PCB* pcb;
  MessageQueue* messagequeue;
  mqd_t mqd;
  struct DescrittorePtr* ptr; 
} Descrittore;

typedef struct DescrittorePtr{
  ListItem list;
  Descrittore* descrittore;
} DescrittorePtr;

void Descrittore_init();
Descrittore* Descrittore_alloc(mqd_t mqd, MessageQueue* mq, PCB* pcb);
int Descrittore_free(Descrittore* d);
Descrittore*  DescrittoreList_byMqd(ListHead* l, mqd_t mqd);
void DescrittoreList_print(ListHead* l);

DescrittorePtr* DescrittorePtr_alloc(Descrittore* descrittore);
int DescrittorePtr_free(DescrittorePtr* d);
void DescrittorePtrList_print(ListHead* l);
