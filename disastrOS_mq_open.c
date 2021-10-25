/*
mq_open
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_OPEN     10
- In disastrOS_syscalls.h: 	void internal_mq_open();

- In disastrOS.h:	
	int disastrOS_mq_open(int messagequeue_id, int type, int msg_size, int max_msg, int open_mode);

- In disastrOS.c
	1:
		syscall_vector[DSOS_CALL_MQ_OPEN]     = internal_mq_open;
  		syscall_numarg[DSOS_CALL_MQ_OPEN]     = 5;
	2:
  int disastrOS_mq_open(int messagequeue_id, int type, int msg_size, int max_msg, int open_mode);{
  	return disastrOS_syscall(DSOS_CALL_MQ_OPEN, messagequeue_id, type, msg_size, max_msg, open_mode);
  }
*/

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disastrOS_pcb.h"
#include "linked_list.h"
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"

void internal_mq_open(){
  int messagequeue_id=running->syscall_args[0];
  int type=running->syscall_args[1];
  int msg_size = running->syscall_args[2];
  int max_msg = running->syscall_args[3];
  int open_mode=running->syscall_args[4];

  MessageQueue* mq=MessageQueueList_byId(&messagequeues_list, messagequeue_id);
  //2 if the resource is opened in CREATE mode, create the resource
  //  and return an error if the resource is already existing
  // otherwise fetch the resource from the system list, and if you don't find it
  // throw an error
  //printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (open_mode&DSOS_CREATE){
    if (mq) {
      running->syscall_retvalue=DSOS_EMESSAGEQUEUECREATE;
      return;
    }
    mq=MessageQueue_alloc(messagequeue_id, type, 0, msg_size, max_msg);
    List_insert(&messagequeues_list, messagequeues_list.last, (ListItem*) mq);
  }

  // at this point we should have the messagequeue, if not something was wrong
  if (! mq || mq->type!=type) {
     running->syscall_retvalue=DSOS_EMESSAGEQUEUEOPEN;
     return;
  }
  
  if (open_mode&DSOS_EXCL && mq->descrittori_ptrs.size){
     running->syscall_retvalue=DSOS_EMESSAGEQUEUENOEXCL;
     return;
  }

  
  //5 create the descriptor for the messagequeue in this process, and add it to
  //  the process descriptor list. Assign to the messagequeue a new fd
  Descrittore* des=Descrittore_alloc(running->last_mqd, mq, running);
  if (! des){
     running->syscall_retvalue=DSOS_EMESSAGEQUEUENOFD;
     return;
  }

  /* !!! Potrebbe darmi problemi il fatto che stia facendo ++ di un valore non int*/


  printf("Aggiunta la mq avente id %d\n", messagequeue_id);
  running->last_mqd++; // we increment the mqd value for the next call
  DescrittorePtr* desptr=DescrittorePtr_alloc(des);
  List_insert(&running->descrittori, running->descrittori.last, (ListItem*) des);
  
  //6 add to the messagequeue, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des->ptr=desptr;
  List_insert(&mq->descrittori_ptrs, mq->descrittori_ptrs.last, (ListItem*) desptr);

  // return the MQD of the new descriptor to the process
  running->syscall_retvalue = des->mqd;
}
