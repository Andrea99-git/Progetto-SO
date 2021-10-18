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
