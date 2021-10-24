/*
mq_receive
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_RECEIVE     14
- In disastrOS_syscalls.h: 	void internal_mq_receive();

- In disastrOS.h:	
  int disastrOS_mq_receive(int mq_descriptor, char* msg_ptr, int msg_len);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_RECEIVE]     = internal_mq_receive;
  		  syscall_numarg[DSOS_CALL_MQ_RECEIVE]     = 3;
	2:
  int disastrOS_mq_receive(int mq_descriptor, char* msg_ptr, int msg_len){
  	return disastrOS_syscall(DSOS_CALL_MQ_RECEIVE, mq_descriptor, msg_ptr, msg_len)
  }

*/
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_descrittore.h"
#include "disastrOS_pcb.h"
#include "linked_list.h"

void internal_mq_receive(){
	int mqd = running -> syscall_args[0];
	char* msg_ptr = running -> syscall_args[1];
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
	
	if (mq -> msg_num == 0){
		running -> syscall_retvalue = DSOS_EMESSAGEQUEUEEMPTY;
		return;
	}

	Message* msg = (Message*) List_detach((ListHead*)&mq -> messages, (ListItem*)(mq-> messages.first));
	assert(msg);

	msg_ptr = (char*) msg -> message;
	running -> syscall_args[1] = msg_ptr;

	printf("AOOOOOOOOOOOOOO %ld\t, %s\t, %ld\n",running->syscall_args[0], running->syscall_args[1], running->syscall_args[2]);

	//printf("Ricevuto messaggio: %s == %s\n",msg->message,(char*)(running -> syscall_args[1]));
	printf("Numero di messaggi in coda = %d\n", mq->msg_num);
	Message_free(msg);
	mq -> msg_num--;
	running -> syscall_retvalue = 0;
	return;

}
