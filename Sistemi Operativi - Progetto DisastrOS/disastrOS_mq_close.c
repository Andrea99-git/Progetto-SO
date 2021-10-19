/*
mq_close
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_CLOSE     11
- In disastrOS_syscalls.h: 	void internal_mq_close();

- In disastrOS.h:	
	int disastrOS_mq_close(mqd_t mq_descriptor);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_CLOSE]     = internal_mq_close;
  		  syscall_numarg[DSOS_CALL_MQ_CLOSE]     = 1;
	2:
		int disastrOS_mq_close(mqd_t mq_descriptor){
  			return disastrOS_syscall(DSOS_CALL_MQ_CLOSE, mq_descriptor)
		}
*/

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"
#include "disastrOS_pcb.h"
#include "linked_list.h"

void internal_mq_close(){
  //1 retrieve the mqd of the message queue to close
  mqd_t mqd=running->syscall_args[0];

  Descriptor* des=DescriptorList_byFd(&running->descrittori, mqd);
  //2 if the mqd is not in the the process, we return an error
  if (! des){
    running->syscall_retvalue=DSOS_EMESSAGEQUEUECLOSE;
    return;
  }

  //3 we remove the descriptor from the process list
  des = (Descrittore*) List_detach(&running->descrittori, (ListItem*) des);
  assert(des);

  MessageQueue* mq=des->messagequeue;

  // we remove the descriptor pointer from the resource list
  DescrittorePtr* desptr=(DescrittorePtr*) List_detach(&res->descrittori_ptrs, (ListItem*)(des->ptr));
  assert(desptr);
  Descrittore_free(des);
  DescrittorePtr_free(desptr);
  running->syscall_retvalue=0;
}
