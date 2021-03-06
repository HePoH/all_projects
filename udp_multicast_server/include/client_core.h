#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#define _GNU_SOURCE

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

#define MULTICAST_SERVER_IP "224.10.10.10"

#define SERVER_IP "192.168.2.1"
#define SERVER_PORT 8888

#define MAX_MSG_SIZE 256

#endif
