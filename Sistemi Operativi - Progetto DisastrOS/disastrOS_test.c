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
  int mqd=disastrOS_mq_open(disastrOS_getpid(),type,15,20,mode);
  printf("mqd=%d\n", mqd);
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;

  printf("I prossimi 3 thread hanno msg_size della mq < della msg_length\n");

  for (int i=0; i<3; ++i) {
   int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int mqd=disastrOS_mq_open(i,type,3, 5,mode);
    printf("mqd=%d\n", mqd);
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
    char* ptr = "prova";
    mqd = disastrOS_mq_send(i,ptr,6);
    char buf[20];
    mqd = disastrOS_mq_receive(i,(char*)buf,6);
    if (mqd == 0){
      printf("Stringa ricevuta: %s\n", buf);
    }
  }

  for (int i=0; i<3; ++i) {
    int fd = disastrOS_closeResource(i);
    if(fd == 0){
      printf("Chiudi risorsa\n");
    }
    int mqd=disastrOS_mq_close(i);
    if(mqd == 0){
      printf("Chiudi message queue\n");
    }
  }


  for (int i=0; i<20; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int mqd=disastrOS_mq_open(i,type,20, 3,mode);
    printf("mqd=%d\n", mqd);
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
    char* ptr = "prova";
    mqd = disastrOS_mq_send(i,ptr,6);
    char buf[20];
    if(i > 8){
      mqd = disastrOS_mq_receive(i,(char*)buf,6);
      if (mqd == 0){
        printf("Stringa ricevuta: %s\n", buf);
      }
    }
  }

  for (int i=0; i<20; ++i) {
    int fd = disastrOS_closeResource(i);
    if(fd == 0){
      printf("Chiudi risorsa\n");
    }
    int mqd=disastrOS_mq_close(i);
    if(mqd == 0){
      printf("Chiudi message queue\n");
    }
  }
/*
  //Test per la mq bloccante
  int mqd=disastrOS_mq_open(30,0,20, 5,DSOS_CREATE);
  printf("mqd=%d\n", mqd);
  alive_children++;

  for (int i=0; i<20; ++i) {
    char* ptr = "prova";
    mqd = disastrOS_mq_send(30,ptr,6);
  }
  for(int i=0;i<20;++i){
    char buf[20];
    mqd = disastrOS_mq_receive(30,(char*)buf,6);
    if (mqd == 0){
      printf("Stringa ricevuta: %s\n", buf);
    }
  }

    mqd=disastrOS_mq_close(30);
    if(mqd == 0){
      printf("Chiudi message queue\n");
    }*/
  




  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!");
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
