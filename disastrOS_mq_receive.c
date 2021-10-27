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

	if(mq -> max_msg == 0){
		printf("The max_msg number should be != 0. Cannot receive a message\n");
		running -> syscall_retvalue = DSOS_EMESSAGESIZE;
		return;
	}


	if (mq -> msg_num == 0){
		if (running->timer) {
			printf("process has already a timer!!!\n");
			running->syscall_retvalue=DSOS_ESLEEP;
			return;
		}
		  int cycles_to_sleep= 25;
		  int wake_time=disastrOS_time+cycles_to_sleep;
		  printf("The msg_num is 0. I'm gonna go sleep and wait that another process sends a message\n");
  
		  TimerItem* new_timer=TimerList_add(&timer_list, wake_time, running);
		  if (! new_timer) {
			printf("no new timer!!!\n");
			running->syscall_retvalue=DSOS_ESLEEP;
			return;
		  } 
		  running->status=Waiting;
		  List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
		  if (ready_list.first){
			running=(PCB*) List_detach(&ready_list, ready_list.first);
		  }
		  else {
			running=0;
			printf ("they are all sleeping\n");
			disastrOS_printStatus();
		  }
		running->syscall_retvalue=DSOS_EMESSAGEQUEUEEMPTY;
		return;
	}


	Message* msg = (Message*) List_detach((ListHead*)&mq -> messages, (ListItem*)(mq-> messages.first));
	assert(msg);

	strcpy(running -> syscall_args[1], msg->message);

	printf("Ricevuto messaggio: %s == %s\n",msg->message,(char*)(running -> syscall_args[1]));
	Message_free(msg);
	mq -> msg_num--;
	running -> syscall_retvalue = 0;
	return;

}
