#ifndef PRINT_N_H
#define PRINT_N_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

#define BUF_SIZE 80
/*char buf[BUF_SIZE];

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;*/

char* print_n(int n);
void* func(void* args);

void make_key();
void dest(void* buf);

#endif

