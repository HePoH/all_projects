#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

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
#include <getopt.h>

#include <netinet/in.h>
#include <netinet/sctp.h>

#include <arpa/inet.h>

#define MAX_MSG_SIZE 256

#define MSG_STREAM  0
#define FILE_STREAM 1

typedef struct server_info {
    int sock;
    struct sockaddr_in addr;
    char filename[64];
} server_info_t;

void *file_hndl(void* args);
void *msg_hndl(void* args);

#endif
