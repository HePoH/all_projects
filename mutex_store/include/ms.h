#ifndef PRINT_N_H
#define PRINT_N_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include <pthread.h>

#define STORE_NUM 5
#define CUSTOMER_NUM 3

pthread_mutex_t* st_mtx;

int* st_val;
int* cst_val;

int get_rand_int(int range_min, int range_max);
void* cst_func(void* args);
void* ldr_func(void* args);

#endif

