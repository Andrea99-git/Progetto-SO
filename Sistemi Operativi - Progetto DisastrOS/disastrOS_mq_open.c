/*
mq_open
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_OPEN     10
- In disastrOS_syscalls.h: 	void internal_mq_open();

- In disastrOS.h:	
	mqd_t disastrOS_mq_open(const char* mq_name, int mq_flag, mode_t mq_mode, struct mq_attr* attr);

- In disastrOS.c
	1:
		syscall_vector[DSOS_CALL_MQ_OPEN]     = internal_mq_open;
  		syscall_numarg[DSOS_CALL_MQ_OPEN]     = 4;
	2:
  mqd_t disastrOS_mq_open(const char* mq_name, int mq_flag, mode_t mq_mode, struct mq_attr* attr){
  	return disastrOS_syscall(DSOS_CALL_MQ_OPEN, mq_name, mq_flag, mq_mode, attr)
  }
*/



#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disastrOS_pcb.h"
#include "linked_list.h"

void internal_mq_open(){
  const char* mq_name = (char*) running->syscall_args[0];
  int flag = running->syscall_args[1];
  mode_t mode = running->syscall_args[2];
  struct mq_attr attr = running->syscall_args[3];

  mqd_t mqd;

  MessageQueue* mq=MessageQueueList_byId(&messagequeues_list, id);
  //2 if the resource is opened in CREATE mode, create the resource
  //  and return an error if the resource is already existing
  // otherwise fetch the resource from the system list, and if you don't find it
  // throw an error
  //printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (flag&DSOS_CREATE){
    if (mq) {
      running->syscall_retvalue=DSOS_EMESSAGEQUEUECREATE;
      return;
    }
    mq=MessageQueue_alloc(id, 0);
    List_insert(&messagequeues_list, messagequeues_list.last, (ListItem*) mq);
  }

  // at this point we should have the messagequeue, if not something was wrong
  if (! mq || mq->type!=type) {
     running->syscall_retvalue=DSOS_EMESSAGEQUEUEOPEN;
     return;
  }
  
  if (open_mode&DSOS_EXCL && mq->descriptors_ptrs.size){
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
  running->last_mqd++; // we increment the mqd value for the next call
  DescrittorePtr* desptr=DescrittorePtr_alloc(des);
  List_insert(&running->descrittori, running->descrittori.last, (ListItem*) des);
  
  //6 add to the messagequeue, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des->ptr=desptr;
  List_insert(&mq->descrittori_ptrs, res->descrittori_ptrs.last, (ListItem*) desptr);

  // return the FD of the new descriptor to the process
  running->syscall_retvalue = des->mqd;
}