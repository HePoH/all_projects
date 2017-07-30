#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _POSIX_C_SOURCE
#define _XOPEN_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

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

#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

typedef struct pseudo_header {
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t resrv;
    u_int8_t proto;
    u_int16_t len;
} __attribute__((packed)) PSEUDO_HEADER;

#define DEFAULT_IF "ens33" /*"eth0"*/

#define DEST_MAC0 0xF0
#define DEST_MAC1 0xDE
#define DEST_MAC2 0xF1
#define DEST_MAC3 0xFE
#define DEST_MAC4 0xED
#define DEST_MAC5 0x58

#define DEST_IP "192.168.2.1"
#define SOURCE_IP "192.168.2.1"

#define DEST_PORT 8888
#define SOURCE_PORT 9009

#define MAX_MSG_SIZE 256
#define MAX_DGRAM_SIZE 1024

void compute_ip_checksum(struct iphdr* iphdr);
void compute_tcp_checksum(struct iphdr* iph, struct tcphdr* tcph);
void compute_udp_checksum(struct iphdr* iph, struct udphdr* udph);
unsigned short compute_checksum(unsigned short* addr, unsigned int count);

void print_ethernet_header(const u_char* frame, int size);
void print_ip_packet(const u_char * , int);
void print_udp_packet(const u_char * , int);
void print_data(const u_char * , int);

#endif
