#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include <netdb.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>

#define SERVER_IP INADDR_ANY
#define SERVER_PORT 8888

#define MAX_MSG_SIZE 256

typedef struct ci {
	int sd_cln;
	struct sockaddr_in cln_addr;
} CLIENT_INFO;

void* cln_tcp_hndl(void* args);

#endif
