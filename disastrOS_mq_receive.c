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
