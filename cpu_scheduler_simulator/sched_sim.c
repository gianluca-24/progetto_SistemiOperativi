#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

typedef struct{
  float quantum;
  float alpha;
} SJF;

typedef struct {
  int quantum;
} SchedRRArgs;


void sched_SJF(FakeOS* os,void* args_){
  SJF* sjf = (SJF*)args_;
  //ordina l'array ready in base al processo che ha l'expected minore
  //col primo ciclo calcolo tutti i predicted
  ListItem* aux=os->ready.first;
  while (aux){
    FakePCB* pcb =(FakePCB*) aux;
    pcb->predicted_burst = sjf->alpha * pcb->actual_burst + (1 - sjf->alpha) * pcb->predicted_burst;
    aux = aux->next;
  }

  //sort della lista ready rispetto al predicted burst
  // ListItem* aux = os->ready.first;
  // while (aux){
  //   FakePCB* pcb =(FakePCB*) aux;
  // }
  //mi serve un doppio ciclo while sul quale navigo con i list item ->next e ->prev.
  //in parallelo a queste iterazioni devo portarmi dietro il cast a pcb per effettuare
  //il confronto tra predicted_Value per sortare l'array. Posso implementarlo con un bubble sort.
  //BUBBLE SORT CON LINKED LIST
//   void bubbleSort(struct Node* head) {
//     int swapped, i;
//     struct Node* ptr1;
//     struct Node* lptr = NULL;

//     // Checking for empty list
//     if (head == NULL)
//         return;

//     do {
//         swapped = 0;
//         ptr1 = head;

//         while (ptr1->next != lptr) {
//             if (ptr1->data > ptr1->next->data) {
//                 swap(ptr1, ptr1->next);
//                 swapped = 1;
//             }
//             ptr1 = ptr1->next;
//         }
//         lptr = ptr1;
//     } while (swapped);
// }

  return;
}

//////////////////////////////////////////////////////////////////////////

void schedRR(FakeOS* os, void* args_){
  SchedRRArgs* args=(SchedRRArgs*)args_;

  // look for the first process in ready
  // if none, return
  if (! os->ready.first)
    return;

  FakePCB* pcb=(FakePCB*) List_popFront(&os->ready);
  os->running=pcb;
  
  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);

  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
  if (e->duration>args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&pcb->events, (ListItem*)qe);
  }
};
//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  FakeOS_init(&os);
  SJF sjf_args;
  sjf_args.alpha = 0.7;

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
        || os.processes.first){
    FakeOS_simStep(&os);
  }
}
