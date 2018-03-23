#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <netinet/in.h>
#include <netinet/sctp.h>

#include <arpa/inet.h>

#define DEF_SERVER_IP "127.0.0.1"
#define DEF_SERVER_PORT 8888

#define MAX_MSG_SIZE 256

#define MSG_STREAM  0
#define FILE_STREAM 1

typedef struct client_info {
	int sock;
	struct sockaddr_in addr;
} client_info_t;

void* cln_hndl(void* args);

#endif
