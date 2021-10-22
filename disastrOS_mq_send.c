/*
mq_send
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_SEND     13
- In disastrOS_syscalls.h: 	void internal_mq_send();

- In disastrOS.h:	
	int disastrOS_mq_send(int mq_descriptor, char *msg_ptr, int msg_len);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_SEND]     = internal_mq_send;
  		  syscall_numarg[DSOS_CALL_MQ_SEND]     = 3;
	2:
  int disastrOS_mq_send(int mq_descriptor, char *msg_ptr, int msg_len){
  	return disastrOS_syscall(DSOS_CALL_MQ_SEND, mq_descriptor, msg_ptr ,msg_len);
  }
*/

//La funzione dovra' scrivere un messaggio sulla coda (gia' aperta). Se la coda e' piena si blocca ed il processo viene messo a dormire

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

void internal_mq_send(){
	int mqd = running -> syscall_args[0];
	char* msg_ptr = running-> syscall_args[1];
	int msg_len = running->syscall_args[2];

	Descrittore* des=DescrittoreList_byMqd(&running->descrittori, mqd);
	//1 if the mqd is not in the the process, we return an error
  	if (! des){
    		running->syscall_retvalue=DSOS_EMESSAGEQUEUECLOSE;
    		return;
  	}
	
	MessageQueue* mq=des->messagequeue;

	//2 if the msg_len is > of the max possible lenght of a message, we return an error
	if (msg_len > mq -> msg_size){
		running -> syscall_retvalue = DSOS_EMESSAGESIZE;
		return;
	}
	
	//3 if the message_queue if already full we wait and re-try. Else we write on the queue.
	if (mq -> msg_num == mq -> max_msg){
		running -> syscall_retvalue = DSOS_EMESSAGEQUEUEFULL;
		return;
	}
	
	Message* msg = Message_alloc(mq->msg_num);
	List_insert(&mq->messages, mq->messages.last, (ListItem*) msg);

	msg->message = msg_ptr;
	mq -> msg_num++;
	running -> syscall_retvalue = 0;
	return;

}
