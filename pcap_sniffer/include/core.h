#ifndef CORE_H
#define CORE_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <pcap.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

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
#define MAX_BUF_SIZE 64
#define HELP_FILTER_FILE "./doc/help_pcap_filter"

typedef struct pseudo_header {
    u_int32_t s_addr;
    u_int32_t d_addr;
    u_int8_t resrv;
    u_int8_t proto;
    u_int16_t len;
} __attribute__((packed)) PSEUDO_HEADER;

int tcp, udp, icmp, igmp, others, total;

void print_usage_help();
void print_filter_help();

char *get_ip_str(const struct sockaddr *sa, char *str, size_t max_len);
int check_data(const u_char *packet, int size, int proto, char *search_str);
void list_net_devs();

void compute_ip_checksum(struct iphdr *iphdr);
void compute_tcp_checksum(struct iphdr *iph, struct tcphdr *tcph);
void compute_udp_checksum(struct iphdr *iph, struct udphdr *udph);
void compute_icmp_checksum(struct icmphdr* icmph);
unsigned short compute_checksum(unsigned short *addr, unsigned int count);

void pckt_hndl(u_char *, const struct pcap_pkthdr *, const u_char *);
void process_ip_packet(const u_char * , int);
void print_ip_packet(const u_char * , int);
void print_tcp_packet(const u_char *  , int);
void print_udp_packet(const u_char * , int);
void print_icmp_packet(const u_char * , int);
void print_data(const u_char*, uint32_t);

#endif
