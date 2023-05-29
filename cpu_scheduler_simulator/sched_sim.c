#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

typedef struct {
  int quantum;
} SJF;

//TO DO
// DONE - Num di cpu da riga di comando;
// DONE - Lunghezza quanto da riga di comando;
// DONE - Predizione del quanto nello scheduler;
// DONE - pulizia codice + commenti

//calcola la predizione
float prediction(FakePCB* pcb){
  pcb->bool = 1;
  return ALPHA * pcb->actual_burst + (1 - ALPHA) * pcb->predicted_burst;
}
//funzione che verifica se c'è almeno una cpu in running per evitare che si
//interrompa la simulazione
int is_running(FakeOS* os){
  for (int i = 0; i < os->num_cpu; i++){
    if (os->cpu_list->running){
      return 1;
    }
  }
  return 0;
}


FakePCB* sched_SJF(FakeOS* os,void* args_){

  SJF* args = (SJF*) args_;

  ListItem* aux = os->ready.first;
  FakePCB* pcb = (FakePCB*) malloc(sizeof(FakePCB));  
  FakePCB* min = (FakePCB*) malloc(sizeof(FakePCB));
  min = (FakePCB*)os->ready.first;
  
  while (aux){
    pcb = (FakePCB*)aux;
    //NUOVA IMPLEMENTAZIONE: semplicemente trovo il min
    pcb->predicted_burst = pcb->bool == 0 ? prediction(pcb) : pcb->predicted_burst;
    min->predicted_burst = min->bool == 0 ? prediction(min) : min->predicted_burst;

    if (pcb->predicted_burst < min->predicted_burst) min = pcb;

    aux = aux->next;
  }
  //funzione che stampa la lista ready con le prediction
  aux = os->ready.first;
  while (aux){
    pcb = (FakePCB*)aux;
    printf("\tProcesso %d prediction %f\n",pcb->pid,pcb->predicted_burst);
    aux = aux->next;
  }

  //gestione del quanto di tempo, ripreso dal codice originale
  ProcessEvent* e = (ProcessEvent*)min->events.first;

  if (e->duration>args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&min->events, (ListItem*)qe);
  }

  List_detach(&os->ready,(ListItem*)min);
  min->actual_burst = 0;
  min->bool = 0;

  return min;
}

int main(int argc, char** argv) {
  FakeOS_init(&os);

  SJF sjf_args;
  
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
  //inserimento lunghezza quanto da tastiera
  fprintf(stdout, "Inserisci il quantum: \n"); 
  fflush(stdout);
  fscanf(stdin, "%d", &sjf_args.quantum);
  fprintf(stdout, "Quantum scelto: %d\n",sjf_args.quantum); 

  os.schedule_args=&sjf_args;
  os.schedule_fn=sched_SJF;

  printf("num processes in queue %d\n", os.processes.size);
  while(os.ready.first
        || os.waiting.first
        || os.processes.first
        || is_running(&os)){
    FakeOS_simStep(&os);
  }
}