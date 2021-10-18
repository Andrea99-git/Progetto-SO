/*
mq_send
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_SEND     13
- In disastrOS_syscalls.h: 	void internal_mq_send();

- In disastrOS.h:	
	int disastrOS_mq_send(mqd_t mq_descriptor, char *mq_ptr, size_t mq_len, unsigned int mq_prio);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_SEND]     = internal_mq_send;
  		  syscall_numarg[DSOS_CALL_MQ_SEND]     = 4;
	2:
  int disastrOS_mq_send(mqd_t mq_descriptor, char *mq_ptr, size_t mq_len, unsigned int mq_prio){
  	return disastrOS_syscall(DSOS_CALL_MQ_SEND, mq_descriptor,mq_ptr,mq_len, mq_prio)
  }
*/
