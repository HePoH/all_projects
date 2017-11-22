#ifndef CORE_H
#define CORE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#include <ifaddrs.h>
#include <net/if.h>

typedef struct _diff_time {
    struct timeval reqs_time;
    struct timeval repl_time;
} diff_time_t;

typedef struct pseudo_header {
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t resrv;
    u_int8_t proto;
    u_int16_t len;
} __attribute__((packed)) PSEUDO_HEADER;

typedef struct _ping_sets {
    uint32_t sock;
    pid_t reqs_id;
    struct sockaddr_in src_addr;
    struct sockaddr_in dst_addr;
    uint32_t reqs_count;
    uint32_t reqs_timeout;
    uint8_t debug_opt;
    diff_time_t *diff_time;
} ping_sets_t;

#define MAX_PACKET_SIZE 1024
#define MAX_HOST_NAME_SIZE 256

#define DEF_SOURCE_IP "192.168.126.131"
#define DEF_DEST_IP "195.93.187.16"

#define DEF_IF_NAME "ens33"

uint32_t stat_pckt_transm, stat_pckt_recive;

int parse_argvs(int argc, char **argv, ping_sets_t *ping_sets);
void print_usage(char **argv);

int get_self_ip(struct in_addr *sin_addr, char *if_name);
void print_if_addr();

double get_diff_time(struct timeval t0, struct timeval t1);

void sig_hndl(int sig_num);
void init_signal();

void compute_ip_checksum(struct iphdr *iphdr);
void compute_icmp_checksum(struct icmphdr *icmph);
unsigned short compute_checksum(unsigned short *addr, unsigned int count);

void print_ip_packet(const u_char * , int);
void print_icmp_packet(const u_char * , int);
void print_data(const u_char * , int);

void *icmp_reqs_hndl(void* args);
void *icmp_repl_hndl(void* args);

#endif
