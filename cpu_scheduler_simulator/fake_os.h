#include "fake_process.h"
#include "linked_list.h"
#define ALPHA 0.5
#pragma once

//aggiunti due campi al FakePCB
//predicted burst salva l'ultimo valore predetto
//actual burst salva il valore effettivo del bursts
//bool serve per identificare se per l'ultimo burst è stata già calcolata la predizione
typedef struct {
  ListItem list;
  int pid;
  ListHead events;
  float predicted_burst;
  float actual_burst;
  int bool;
} FakePCB;

//aggiungo la struct CPU_core
typedef struct {
  FakePCB* running;
} CPU_core;

struct FakeOS;
typedef FakePCB* (*ScheduleFn)(struct FakeOS* os, void* args);

//aggiungo la lista di CPU_core per modellare il sistema come multicore
typedef struct FakeOS{
  FakePCB* running;
  ListHead ready;
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;
  ListHead processes;
  CPU_core* cpu_list;
  int num_cpu;
} FakeOS;

void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
FakePCB* sched_SJF(FakeOS* os,void* args_);
int is_running(FakeOS* os);
float prediction(FakePCB* pcb);
