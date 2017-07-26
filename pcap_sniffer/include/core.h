#ifndef CORE_H
#define CORE_H

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

#include <pcap.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define MAX_SNAP_LEN 65535

typedef struct pseudo_header {
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t resrv;
    u_int8_t proto;
    u_int16_t len;
} __attribute__((packed)) PSEUDO_HEADER;

int tcp, udp, icmp, igmp, others, total;

void list_net_devs();

void compute_ip_checksum(struct iphdr* iphdr);
void compute_tcp_checksum(struct iphdr* iph, struct tcphdr* tcph);
void compute_udp_checksum(struct iphdr* iph, struct udphdr* udph);
unsigned short compute_checksum(unsigned short* addr, unsigned int count);

void pckt_hndl(u_char *, const struct pcap_pkthdr *, const u_char *);
void process_ip_packet(const u_char * , int);
void print_ip_packet(const u_char * , int);
void print_tcp_packet(const u_char *  , int);
void print_udp_packet(const u_char * , int);
void print_icmp_packet(const u_char * , int);
void print_data(const u_char * , int);

#endif
