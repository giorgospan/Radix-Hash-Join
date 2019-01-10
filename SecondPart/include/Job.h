#ifndef JOB_H
#define JOB_H

#include <stdint.h>
#include "Intermediate.h"

struct Job{
  // Function that the worker thread is going to execute
  void (*function)(void*);
  // Argument passed to the function
  void *argument;
};

struct histArg{
  unsigned start;
  unsigned end;
  unsigned* histogram;
};

struct partitionArg{
  unsigned start;
  unsigned end;
  RadixHashJoinInfo *info;
};

struct buildArg{
  unsigned bucket;
  RadixHashJoinInfo *info;
};

struct joinArg{
  unsigned start;
  unsigned end;
  RadixHashJoinInfo *left;
  RadixHashJoinInfo *right;
  struct Vector *results;
};


void histFunc(void* arg);
void partitionFunc(void* arg);
void buildFunc(void* arg);
void joinFunc(void* arg);

//////////////////////////////////////
#define N 10000000
unsigned column[N];
//////////////////////////////////////

#endif
