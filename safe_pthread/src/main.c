#include "../include/print_n.h"

#define PTH_N 10

int main() {
	char* buf = NULL;
	int i = 0, statuses[PTH_N];
	pthread_t tids[PTH_N];

	buf = print_n(15);
	printf("Main --> %s", buf);

	for (i = 0; i < PTH_N; i++)
		pthread_create(&tids[i], NULL, func, (void*) i);

	for (i = 0; i < PTH_N; i++)
		pthread_join(tids[i], (void*)&statuses[i]);

	printf("Main --> %s\n", buf);

	exit(0);
}
