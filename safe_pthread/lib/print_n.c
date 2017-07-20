#include "../include/print_n.h"

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void* func(void* args) {
        char* buf = NULL;
        int tid = 0;

        tid = (int) args;

        buf = print_n(tid);
        printf("Pthread[%d]: %s\n", tid, buf);

        pthread_exit(0);
}

char* print_n(int n) {
	char* buf = NULL;

	pthread_once(&key_once, make_key);

	buf = pthread_getspecific(key);
	if (buf == NULL) {
		buf = malloc(BUF_SIZE * sizeof(char));
		pthread_setspecific(key, buf);
    	}

	snprintf(buf, BUF_SIZE, "num: %d (%p)\n", n, buf);
	return buf;
}

void make_key() {
    pthread_key_create(&key, dest);
}

void dest(void* buf) {
	free(buf);
}
