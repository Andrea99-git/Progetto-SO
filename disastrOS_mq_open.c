/*
mq_open
- In disastrOS_constants.h:	#define DSOS_CALL_MQ_OPEN     10
- In disastrOS_syscalls.h: 	void internal_mq_open();

- In disastrOS.h:	
	mqd_t disastrOS_mq_open(const char* mq_name, int mq_flag, mode_t mq_mode, struct mq_attr* attr);

- In disastrOS.c
	1:
		syscall_vector[DSOS_CALL_MQ_OPEN]     = internal_mq_open;
  		syscall_numarg[DSOS_CALL_MQ_OPEN]     = 4;
	2:
  mqd_t disastrOS_mq_open(const char* mq_name, int mq_flag, mode_t mq_mode, struct mq_attr* attr){
  	return disastrOS_syscall(DSOS_CALL_MQ_OPEN, mq_name, mq_flag, mq_mode, attr)
  }
*/
