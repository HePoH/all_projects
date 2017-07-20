#include "../include/mt.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* pth_func(void* args) {
	int i = 0, ln = 0, tid = 0;
	tid = (int) args;

	pthread_mutex_lock(&mtx);

	for (i = 0; i < 1000000; i++) {
		ln = gn;
		ln++;
		gn = ln;
	}

	pthread_mutex_unlock(&mtx);

        printf("Pthread[%d]: up gn to %d\n", tid, gn);
	pthread_exit(EXIT_SUCCESS);
}
