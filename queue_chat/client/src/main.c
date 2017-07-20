#include "../include/core.h"
/*#include "../include/graph.h"
#include "../include/popup.h"*/

int main(){
	pthread_t srv_tid, cln_inc_tid, cln_out_tid;
	int *srv_stat, *cln_inc_stat, *cln_out_stat;
	char* cln_name;

	cln_name = calloc(MAX_NAME_SIZE, sizeof(char));
	if (cln_name == NULL) {
		perror("Client: malloc");
		exit(EXIT_FAILURE);
	}

	snprintf(cln_name, MAX_NAME_SIZE, "client-%d", getpid());
	printf("Client start\nClient name: %s\n", cln_name);

	pthread_create(&srv_tid, NULL, srv_event_hndl, (void*)cln_name);
	/*pthread_create(&cln_inc_tid, NULL, cln_inc_msg_hndl, (void*)cln_name);
        pthread_create(&cln_out_tid, NULL, cln_out_msg_hndl, (void*)cln_name);*/

	pthread_join(srv_tid, (void**)&srv_stat);
	/*pthread_join(cln_inc_tid, (void**)&cln_inc_stat);
	pthread_join(cln_out_tid, (void**)&cln_out_stat);*/

	printf("Client: main thread bye\n");
	exit(EXIT_SUCCESS);
}
