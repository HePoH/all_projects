#include "../include/ms.h"

/*pthread_mutex_t* st_mtx;*/

int get_rand_int(int range_min, int range_max) {
	if (range_min > range_max) {
		fprintf(stderr, "gen_random(): invalid arguments\n" );
		exit(EXIT_FAILURE);
	}

	return range_min+rand()%(range_max-range_min+1);
}

void* cst_func(void* args) {
	int i = 0, cid = 0, st_id = 0, stf_num = 0;
	cid = (int) args;

	while (cst_val[cid] > 0) {
		st_id = get_rand_int(0, STORE_NUM - 1);
		stf_num = get_rand_int(0, 512);
		stf_num = (cst_val[cid] - stf_num >= 0) ? stf_num : cst_val[cid];

		pthread_mutex_lock(&st_mtx[st_id]);

		stf_num = (st_val[st_id] - stf_num >= 0) ? stf_num : st_val[st_id];
		st_val[st_id] -= stf_num;
		cst_val[cid] -= stf_num;

		printf("Customer[id = %d]: store id = %d, stuff num = %d, remains = %d\n", cid, st_id, stf_num, cst_val[cid]);

		pthread_mutex_unlock(&st_mtx[st_id]);
		sleep(2);
	}

	pthread_exit(EXIT_SUCCESS);
}

void* ldr_func(void* args) {
	int i = 0, lid = 0, st_id = 0, stf_num = 0;
	lid = (int) args;
	while (1) {
		st_id = get_rand_int(0, STORE_NUM - 1);
		stf_num = get_rand_int(0, 512);

		pthread_mutex_lock(&st_mtx[st_id]);

		st_val[st_id] += stf_num;
		printf("\nLoader  [id = %d]: store id = %d, stuff num = %d, balance = %d\n\n", lid, st_id, stf_num, st_val[st_id]);

		pthread_mutex_unlock(&st_mtx[st_id]);
		sleep(4);
	}

	pthread_exit(EXIT_SUCCESS);
}
