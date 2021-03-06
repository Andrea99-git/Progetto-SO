/*
mq_unlink
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_UNLINK     12
- In disastrOS_syscalls.h: 	void internal_mq_unlink();

- In disastrOS.h:	
	int disastrOS_mq_unlink(int id);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_UNLINK]     = internal_mq_unlink;
  		  syscall_numarg[DSOS_CALL_MQ_UNLINK]     = 1;
	2:
		int disastrOS_mq_unlink(int messagequeue_id){
  			return disastrOS_syscall(DSOS_CALL_MQ_UNLINK, messagequeue_id)
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
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"

void internal_mq_unlink(){
  int id=running->syscall_args[0];

  // find the messagequeue in with the id
  MessageQueue* mq=MessageQueueList_byId(&messagequeues_list, id);
  if (! mq){
    running->syscall_retvalue=DSOS_EMESSAGEQUEUECLOSE;
    return;
  }

  // ensure the messagequeue is not used by any process
  if(mq->descrittori_ptrs.size){
    running->syscall_retvalue=DSOS_EMESSAGEQUEUEINUSE;
    return;
  }
/*
  // free each message that has not been received
  for (int i = 0; i < mq -> msg_num; i++){
    Message* msg = (Message*) List_detach((ListHead*)&mq -> messages, (ListItem*)(mq-> messages.first));
    assert(msg);
    Message_free(msg);
    mq -> msg_num--;
  }
*/
  mq=(MessageQueue*) List_detach(&messagequeues_list, (ListItem*) mq);
  assert(mq);
  MessageQueue_free(mq);
  running->syscall_retvalue=0;
}
