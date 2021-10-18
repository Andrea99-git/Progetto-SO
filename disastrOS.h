#pragma once
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disastrOS_pcb.h"
#include "linked_list.h"

#ifdef _DISASTROS_DEBUG_
#include <stdio.h>

#define disastrOS_debug(...) printf(__VA_ARGS__) 

#else //_DISASTROS_DEBUG_

#define disastrOS_debug(...) ;

#endif //_DISASTROS_DEBUG_

// initializes the structures and spawns a fake init process
void disastrOS_start(void (*f)(void*), void* args, char* logfile);

// generic syscall 
int disastrOS_syscall(int syscall_num, ...);

// classical process control
int disastrOS_getpid(); // this should be a syscall, but we have no memory separation, so we return just the running pid
int disastrOS_fork();
void disastrOS_exit(int exit_value);
int disastrOS_wait(int pid, int* retval);
void disastrOS_preempt();
void disastrOS_spawn(void (*f)(void*), void* args );
void disastrOS_shutdown();

// timers
void disastrOS_sleep(int);

// respurces (files)
int disastrOS_openResource(int resource_id, int type, int mode);
int disastrOS_closeResource(int fd) ;
int disastrOS_destroyResource(int resource_id);

//IPC
mqd_t disastrOS_mq_open(const char* mq_name, int mq_flag, mode_t mq_mode, struct mq_attr* attr);
int disastrOS_mq_close(mqd_t mq_descriptor);
int disastrOS_mq_unlink(const char* mq_name);
int disastrOS_mq_send(mqd_t mq_descriptor, char *mq_ptr, size_t mq_len, unsigned int mq_prio);
ssize_t disastrOS_mq_receive(mqd_t mq_descriptor, char* mq_ptr, size_t mq_len, unsigned int *mq_prio);


// debug function, prints the state of the internal system
void disastrOS_printStatus();