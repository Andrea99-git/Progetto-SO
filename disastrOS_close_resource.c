#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

/*
	Riceve in ingresso il file descriptor fd della risorsa da chiudere, la rimuove dalla lista dei
	descrittori aperti (running->descriptors), la rimuove inoltre dalla lista dei puntatori di 
	descrittori. Restituisce infine (se tutto e' andato correttamente) 0.
*/

void internal_closeResource(){
  //1 retrieve the fd of the resource to close
  int fd=running->syscall_args[0];

  Descriptor* des=DescriptorList_byFd(&running->descriptors, fd);
  //2 if the fd is not in the the process, we return an error
  if (! des){
    running->syscall_retvalue=DSOS_ERESOURCECLOSE;
    return;
  }

  //3 we remove the descriptor from the process list
  des = (Descriptor*) List_detach(&running->descriptors, (ListItem*) des);
  assert(des);

  Resource* res=des->resource;

  // we remove the descriptor pointer from the resource list
  DescriptorPtr* desptr=(DescriptorPtr*) List_detach(&res->descriptors_ptrs, (ListItem*)(des->ptr));
  assert(desptr);
  Descriptor_free(des);
  DescriptorPtr_free(desptr);
  running->syscall_retvalue=0;
}
