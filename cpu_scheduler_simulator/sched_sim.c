#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

typedef struct{
  float alpha;
} SJF;

typedef struct {
  int quantum;
} SchedRRArgs;

int is_running(FakeOS* os){
  for (int i = 0; i < NUM_CPU; i++){
    if (os->cpu_list->running){
      return 1;
    }
  }
  return 0;
}


// void swap_elem(FakeOS* os, ListItem* item1, ListItem* item2) {
//   printf("swap\n");
//   if (os->ready.size < 2) return;

//   if (item2->next != NULL && item1->prev != NULL){
//       item1->next = item2->next;
//       item2->prev = item1->prev;
//       if (item1->next != NULL) {
//           item1->next->prev = item1;
//       }
//       if (item2->prev != NULL) {
//           item2->prev->next = item2;
//       }
//       item1->prev = item2;
//       item2->next = item1;
//   } else if(item2->next == NULL && item1->prev == NULL){
//     //ci sono solo due elementi
//       printf("Ci sono solo due elementi\n");
//       ListItem* prev1 = item1->prev;
//       ListItem* next1 = item1->next;
//       ListItem* prev2 = item2->prev;
//       ListItem* next2 = item2->next;

//       if (prev1 != NULL) prev1->next = item2;
//       // else list->first = item2;
//       else os->ready.first = item2;

//       // if (next1 != NULL) next1->prev = item2;
//       // else list->last = item2;

//       // if (prev2 != NULL) prev2->next = item1;
//       // else list->first = item1;

//       // if (next2 != NULL) next2->prev = item1;
//       // else list->last = item1;

//       // item1->prev = prev2;
//       // item1->next = next2;
//       // item2->prev = prev1;
//       // item2->next = next1;
//     }
    
//     return;
// }

void sched_SJF(FakeOS* os,void* args_){
  // printf("ciao\n");
  if (!os->ready.first || !os->ready.first->next) return;

  SJF* sjf = (SJF*)args_;

  ListItem* aux=os->ready.first;
  FakePCB* aux_pcb=(FakePCB*) malloc(sizeof(FakePCB));
  
  // float pred;
  // //probabilmente è sbagliato calcolare la predizione ogni volta che si chiama lo scheduler
  // while (aux){
  //   // printf("while aggiornamento predicted\n");
  //   aux_pcb =(FakePCB*) aux;
  //   ProcessEvent* e=(ProcessEvent*) aux_pcb->events.first;
  //   aux_pcb->actual_burst = e->duration;
  //   assert(e->type==CPU);
  //   //previsione del burst
  //   pred = aux_pcb->predicted_burst;
  //   aux_pcb->predicted_burst = sjf->alpha * aux_pcb->actual_burst + (1 - sjf->alpha) * pred;
  //   printf("predicted burst proc %d: %f\n",aux_pcb->pid, aux_pcb->predicted_burst);
  //   aux = aux->next;
  // }

  ////////////////////////
  //DOMANI RICOMINCIARE DA QUA!!!
  //SCORRERE LA LISTA DAI PCB E INVERTIRE I PUNTATORI DEL PCB
  // aux=os->ready.first;
  // aux_pcb =(FakePCB*) aux;
  // while (aux_pcb){
  //   printf("pid processo:  %d\n", aux_pcb->pid);
  //   aux_pcb = (FakePCB*) aux_pcb->list.next;
  // }
  ////////////////////////

  int swap;
  ListItem* item1;
  ListItem* item2 = NULL;

  do{
    swap = 0;
    item1 = os->ready.first;

    while (item1->next != item2){
      FakePCB* curr_pcb = (FakePCB*)item1;
      FakePCB* next_pcb = (FakePCB*)item1->next;
      if (curr_pcb->predicted_burst > next_pcb->predicted_burst){
        printf("swap\n");
        //TO DO
        swap = 1;
      }
      item1 = item1->next;
    }
    item2 = item1;
  } while (swap);

  //verifica swap
  aux=os->ready.first;
  while (aux){
    aux_pcb =(FakePCB*) aux;
    printf("predicted burst proc %d: %f\n",aux_pcb->pid, aux_pcb->predicted_burst);
    aux = aux->next;
  }
  return;
}

int main(int argc, char** argv) {
  FakeOS_init(&os);
  SJF sjf_args;
  sjf_args.alpha = 0.5;

  os.schedule_args=&sjf_args;
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
