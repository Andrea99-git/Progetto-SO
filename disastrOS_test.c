#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include <stdlib.h>

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;

  int mqd=disastrOS_mq_open(disastrOS_getpid(),type,20,10,mode);
  if (mqd < 0) {
    printf("Child %d - Cannot open a message queue in child number: %d. Error number: %d\n", disastrOS_getpid(), disastrOS_getpid(), mqd);
  }
  disastrOS_printStatus();
  
  int ret = disastrOS_mq_close(mqd);
  if (ret < 0) {
    printf("Child %d - Cannot close message queue in child number: %d\n", disastrOS_getpid(), disastrOS_getpid());
  }
  disastrOS_printStatus();

  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep(2);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  
  int alive_children = 0;

  printf("Main - I am gonna create 10 message queues\n\n");

  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;

    printf("Main - Opening messagequeue having mqd: %d (and creating if necessary)\n", i);

    int mqd=disastrOS_mq_open(i,type,20, 10,mode);
    if (mqd < 0) {
      printf("Main - Cannot open a message queue in main process: %d\n", disastrOS_getpid());
    }

    printf("Main - Sending 5 messages to message queue having mqd = %d\n", i);

    for (int j = 0; j < 5; j++){
      char msg_ptr[20];
      sprintf(msg_ptr,"msg %d", j);
      int ret = disastrOS_mq_send(i, msg_ptr, 5);
      if (ret < 0){
        printf("Main - Cannot send a message. Error number = %d.\n", ret);
      }
    } 
  }

  disastrOS_printStatus();

  for (int i=0; i<10; ++i) {
    printf("Main - Receiving 5 messages to message queue having mqd = %d\n", i);

    for (int j = 0; j < 5; j++){
      char msg_ptr[20];
      int ret = disastrOS_mq_receive(i, msg_ptr, 20);
      if (ret < 0){
        printf("Main - Cannot receive a message. Error number = %d.\n", ret);
      }
    } 
  }

  disastrOS_printStatus();

  for (int i=0; i<10; ++i) {
      printf("Main - Sending 5 messages to message queue having mqd = %d\n", i);

    for (int j = 0; j < 5; j++){
      char msg_ptr[20];
      sprintf(msg_ptr,"msg %d", j);
      int ret = disastrOS_mq_send(i, msg_ptr, 5);
      if (ret < 0){
        printf("Main - Cannot send a message. Error number = %d.\n", ret);
      }
    } 
  }

  disastrOS_printStatus();

  printf("Main - Closing every message queue i created\n");

  for (int i=0; i<10; ++i) {
    printf("Main - Currently closing message queue having mqd: %d\n", i);
    int ret=disastrOS_mq_close(i);

    if (ret < 0){
      printf("Main: Cannot close the message queue. Error obtaied: %d", ret);
    }
  }

  printf("\n\n");
  disastrOS_printStatus();


  for(int i = 2; i < 5; i++){
    printf("Main - Spawning process number %d\n", i);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  } 

  printf("\n\n");
  disastrOS_printStatus();

  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
     pid, retval, alive_children);
    --alive_children;
  }

  disastrOS_printStatus();


  printf("Main : I'm gonna unlink every message queue\n");
  for (int i = 0; i < 10; i++){
    printf("Main - Currently unlinking message queue having mqd: %d\n", i);
    
    int ret=disastrOS_mq_unlink(i);

    if (ret < 0){
      printf("Main - Cannot unlink the message queue. Error obtaied: %d", ret); 
    }
  } 
  
  disastrOS_printStatus();

  printf("shutdown!\n");
  disastrOS_shutdown();
}


int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process

  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
