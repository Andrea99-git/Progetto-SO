/*
mq_unlink
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_UNLINK     12
- In disastrOS_syscalls.h: 	void internal_mq_unlink();

- In disastrOS.h:	
	int disastrOS_mq_unlink(const char* mq_name);

- In disastrOS.c
	1:
		  syscall_vector[DSOS_CALL_MQ_UNLINK]     = internal_mq_unlink;
  		  syscall_numarg[DSOS_CALL_MQ_UNLINK]     = 1;
	2:
		int disastrOS_mq_unlink(const char* mq_name){
  			return disastrOS_syscall(DSOS_CALL_MQ_UNLINK, mq_name)
		}
*/
