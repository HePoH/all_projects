#ifndef CORE_H
#define CORE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

typedef struct pseudo_header {
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t resrv;
    u_int8_t proto;
    u_int16_t len;
} __attribute__((packed)) PSEUDO_HEADER;

/*#define SOURCE_IP "192.168.2.1"
#define DEST_IP "192.168.2.1"*/

/*#define SOURCE_IP "192.168.126.131"
#define DEST_IP "192.168.126.131"*/

#define SOURCE_IP "192.168.2.1"
#define DEST_IP "195.93.187.16"

#define MAX_MSG_SIZE 256
#define MAX_PACKET_SIZE 1024

#define MAX_PACKET_NUM 10

void compute_ip_checksum(struct iphdr* iphdr);
void compute_icmp_checksum(struct icmphdr* icmph);
unsigned short compute_checksum(unsigned short* addr, unsigned int count);

void print_ip_packet(const u_char * , int);
void print_icmp_packet(const u_char * , int);
void print_data(const u_char * , int);

void* reqs_hndl(void* args);
void* repl_hndl(void* args);

#endif
