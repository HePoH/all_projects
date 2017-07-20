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
#include <errno.h>

#include <arpa/inet.h>

#define SERVER_IP "192.168.2.255"
#define SERVER_PORT 8888

#define MAX_MSG_SIZE 256

#endif
