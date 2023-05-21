#include "fake_process.h"
#include "linked_list.h"
#define NUM_CPU 5
#pragma once

typedef struct {
  ListItem list;
  int pid;
  ListHead events;
} FakePCB;

typedef struct {
  FakePCB* running;
  //quantum
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
