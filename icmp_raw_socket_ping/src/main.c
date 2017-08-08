#include "../include/core.h"

/* https://cboard.cprogramming.com/networking-device-communication/107801-linux-raw-socket-programming.html */

int main() {
	int sd = 0, on = 1, rtn = 0;
    pthread_t reqs_hndl_tid, repl_hndl_tid;

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sd == -1) {
		perror("socket");

		close(sd);
		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if (rtn == -1) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    rtn = pthread_create(&reqs_hndl_tid, NULL, reqs_hndl, (void*)&sd);
    if(rtn != 0) {
        printf("pthread_create");
        exit(EXIT_FAILURE);
    }

    rtn = pthread_create(&repl_hndl_tid, NULL, repl_hndl, (void*)&sd);
    if(rtn != 0) {
        printf("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_join(reqs_hndl_tid, NULL);
    pthread_join(repl_hndl_tid, NULL);

    exit(EXIT_SUCCESS);
}
