#include "fake_process.h"
#include "linked_list.h"
#define NUM_CPU 1
#define ALPHA 0.5
#pragma once

typedef struct {
  ListItem list;
  int pid;
  ListHead events;
  //predicted burst salva l'ultimo valore predettp
  float predicted_burst;
  //actual burst salva il valore effettivo del bursts
  int actual_burst;
} FakePCB;

typedef struct {
  FakePCB* running;
} CPU_core;

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
  FakePCB* running;
  ListHead ready;
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;
  ListHead processes;
  CPU_core cpu_list[NUM_CPU];
} FakeOS;

void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
void sched_SJF(FakeOS* os,void* args_);
// void swap_elem(FakeOS* os, ListItem* item1, ListItem* item2);
void swap_elem(FakeOS* os, FakePCB* curr_pcb, FakePCB* next_pcb);
int is_running(FakeOS* os);
int all_running(FakeOS* os);
