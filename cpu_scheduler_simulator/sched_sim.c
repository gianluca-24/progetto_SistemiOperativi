#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

int is_running(FakeOS* os){
  for (int i = 0; i < NUM_CPU; i++){
    if (os->cpu_list->running){
      return 1;
    }
  }
  return 0;
}

void sched_SJF(FakeOS* os,void* args_){
  // printf("ciao\n");
  if (!os->ready.first || !os->ready.first->next) return;

  ListItem* aux = os->ready.first;
  FakePCB* pcb = (FakePCB*) malloc(sizeof(FakePCB));
  //mette la lista dei progetti
  while(aux){
    pcb = (FakePCB*)aux;
    printf("Burst predicted processo: %d, %f\n", pcb->pid, pcb->predicted_burst);
    aux = aux->next;
  }
  aux = os->ready.first;
  while (aux){
    pcb = (FakePCB*)aux;
    if (pcb->predicted_burst < ((FakePCB*)os->ready.first)->predicted_burst){
      List_detach(&os->ready,(ListItem*)pcb);
      List_pushFront(&os->ready,(ListItem*)pcb);
    }
    aux = aux->next;
  }
  return;
}

int main(int argc, char** argv) {
  FakeOS_init(&os);
  os.schedule_fn=sched_SJF;
  
  for (int i=1; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("num processes in queue %d\n", os.processes.size);
  while(os.running
        || os.ready.first
        || os.waiting.first
        || os.processes.first
        || is_running(&os)){
    FakeOS_simStep(&os);
  }
}
