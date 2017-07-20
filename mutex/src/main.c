#include "../include/mt.h"

#define PTH_N 10

int main() {
	int i = 0, statuses[PTH_N];
	pthread_t tids[PTH_N];

	gn = 0;

	for (i = 0; i < PTH_N; i++)
		pthread_create(&tids[i], NULL, pth_func, (void*) i);

	for (i = 0; i < PTH_N; i++)
		pthread_join(tids[i], (void**)&statuses[i]);

	printf("Global number: %d\n", gn);
	exit(EXIT_SUCCESS);
}
