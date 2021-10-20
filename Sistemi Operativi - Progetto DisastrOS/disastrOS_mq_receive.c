/*
mq_receive
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_RECEIVE     14
- In disastrOS_syscalls.h: 	void internal_mq_receive();

- In disastrOS.h:	
  ssize_t disastrOS_mq_receive(mqd_t mq_descriptor, char* mq_ptr, size_t mq_len, unsigned int *mq_prio);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_RECEIVE]     = internal_mq_receive;
  		  syscall_numarg[DSOS_CALL_MQ_RECEIVE]     = 4;
	2:
  ssize_t disastrOS_mq_receive(mqd_t mq_descriptor, char* mq_ptr, size_t mq_len, unsigned int *mq_prio){
  	return disastrOS_syscall(DSOS_CALL_MQ_RECEIVE, mq_descriptor, mq_ptr, mq_len, mq_prio)
  }

*/
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"
#include "disastrOS_pcb.h"
#include "linked_list.h"

void internal_mq_receive(){
	mqd_t mqd = running -> syscall_args[0];
	char* msg_ptr = running-> syscall_args[1];
	int msg_len = running->syscall_args[2];

	Descrittore* des=DescrittoreList_byMqd(&running->descrittori, mqd);
	//1 if the mqd is not in the the process, we return an error
  	if (! des){
    		running->syscall_retvalue=DSOS_EMESSAGEQUEUECLOSE;
    		return;
  	}
	
	MessageQueue* mq=des->messagequeue;

	//2 if the lenght of the buffer for the message is < than the msg_size, we return an error
	if (msg_len < mq -> msg_size){
		running -> syscall_retvalue = DSOS_EMESSAGESIZE;
	}
	
	//3 if the number of messages in the mq is 0 we wait and re-try. Else we write on the queue.
	while(1){
		if (mq -> nummsg > 0){
			strcpy (msg_ptr, mq->buffer[nummsg];
			msg_ptr = mq -> buffer[nummsg]; 	//potrei dover usare strcpy
			mq -> nummsg--;
			running -> syscall_retvalue = 1;
			return;
		}
		usleep(1.125 * 1e6);
	}
}
