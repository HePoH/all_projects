#ifndef PRINT_N_H
#define PRINT_N_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

int gn;
void* pth_func(void* args);

#endif

