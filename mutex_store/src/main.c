#include "../include/ms.h"

int main() {
	int i = 0, **statuses;
	pthread_t ldr_id, cst_ids[CUSTOMER_NUM];

	srand(time(NULL));

	printf("\n");
	st_val = malloc(STORE_NUM * sizeof(int));
	if (st_val == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < STORE_NUM; i++) {
		st_val[i] = get_rand_int(800, 1200);
		printf("Store[id = %d]: value = %d\n", i, st_val[i]);
	}

	printf("\n");
	cst_val = malloc(CUSTOMER_NUM * sizeof(int));
	if (cst_val == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < CUSTOMER_NUM; i++) {
		cst_val[i] = get_rand_int(2800, 3200);
		printf("Customer[id = %d]: value = %d\n", i, cst_val[i]);
	}

	printf("\n");
	st_mtx = malloc(STORE_NUM * sizeof(pthread_mutex_t));
	if (st_mtx == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < STORE_NUM; i++) {
		/*st_mtx[i] = PTHREAD_MUTEX_INITIALIZER;*/
		pthread_mutex_init(&st_mtx[i], NULL);
		printf("Store[id = %d]: mutex init successful\n", i);
	}

	statuses = malloc(CUSTOMER_NUM * sizeof(int*));
	if (cst_val == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	pthread_create(&ldr_id, NULL, ldr_func, (void*) 0);

	for (i = 0; i < CUSTOMER_NUM; i++)
		pthread_create(&cst_ids[i], NULL, cst_func, (void*) i);

	for (i = 0; i < CUSTOMER_NUM; i++)
		pthread_join(cst_ids[i], (void**) &statuses[i]);

	pthread_cancel(ldr_id);

	exit(EXIT_SUCCESS);
}
