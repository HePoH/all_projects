#include "../include/client_core.h"

void compute_ip_checksum(struct iphdr* iph) {
	iph->check = 0;
	iph->check = compute_checksum((unsigned short*)iph, iph->ihl<<2);
}

void compute_tcp_checksum(struct iphdr* iph, struct tcphdr* tcph) {
	unsigned short iphlen = 0, tcpplen = 0;
	unsigned char* block = NULL;
	PSEUDO_HEADER* ph = NULL;

	iphlen = iph->ihl*4;
	tcpplen = ntohs(iph->tot_len) - iphlen;

	ph = malloc(sizeof(PSEUDO_HEADER));
	if (ph == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	ph->s_addr = iph->saddr;
	ph->d_addr = iph->daddr;
	ph->resrv = 0;
	ph->proto = iph->protocol;
	ph->len = htons(tcpplen);

	block = malloc(sizeof(PSEUDO_HEADER) + tcpplen);
	if (block == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	tcph->check = 0;

	memcpy(block, ph, sizeof(PSEUDO_HEADER));
	memcpy(block + sizeof(PSEUDO_HEADER), tcph, tcpplen);

	tcph->check = compute_checksum((unsigned short*)block, sizeof(PSEUDO_HEADER) + tcpplen);
}

void compute_udp_checksum(struct iphdr* iph, struct udphdr* udph) {
	unsigned short iphlen = 0, udpplen = 0;
	unsigned char* block = NULL;
	PSEUDO_HEADER* ph = NULL;

	iphlen = iph->ihl*4;
	udpplen = ntohs(udph->len);

	ph = malloc(sizeof(PSEUDO_HEADER));
	if (ph == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	ph->s_addr = iph->saddr;
	ph->d_addr = iph->daddr;
	ph->resrv = 0;
	ph->proto = iph->protocol;
	ph->len = udph->len;

	block = malloc(sizeof(PSEUDO_HEADER) + udpplen);
	if (block == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	udph->check = 0;

	memcpy(block, ph, sizeof(PSEUDO_HEADER));
	memcpy(block + sizeof(PSEUDO_HEADER), udph, udpplen);

	udph->check = compute_checksum((unsigned short*)block, sizeof(PSEUDO_HEADER) + udpplen);
}

unsigned short compute_checksum(unsigned short* addr, unsigned int count) {
	unsigned long sum = 0;

	while (count > 1) {
		sum += *addr++;
		count -= 2;
	}

	if(count > 0)
		sum += ((*addr)&htons(0xFF00));

	while (sum>>16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}

	sum = ~sum;
	return ((unsigned short)sum);
}

void print_ethernet_header(const u_char* frame, int size) {
	struct ethhdr *eth = NULL;
	eth = (struct ethhdr *)frame;

	printf("\n");
	printf("Ethernet Header\n");
	printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
	printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
	printf("   |-Protocol            : %u \n", (unsigned short)eth->h_proto);
}

void print_ip_header(const u_char* packet, int size) {
	unsigned short iphdrlen = 0, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct sockaddr_in source, dest;

	iph = (struct iphdr *)(packet + sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;

	print_ethernet_header(packet, size);

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;

	check_sum_back = iph->check;
	compute_ip_checksum(iph);

	printf("\n");
	printf("IP Header\n");
	printf("   |-IP Version        : %d\n", (unsigned int)iph->version);
	printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
	printf("   |-Type Of Service   : %d\n", (unsigned int)iph->tos);
	printf("   |-IP Total Length   : %d Bytes(Size of Packet)\n", ntohs(iph->tot_len));
	printf("   |-Identification    : %d\n", ntohs(iph->id));
	/*printf("   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);*/
	/*printf("   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);*/
	/*printf("   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);*/
	printf("   |-TTL               : %d\n", (unsigned int)iph->ttl);
	printf("   |-Protocol          : %d\n", (unsigned int)iph->protocol);
	printf("   |-Checksum          : %d (verif: %d) [%s]\n", ntohs(check_sum_back), ntohs(iph->check), (check_sum_back == iph->check) ? "valid" : "invalid");
	printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
	printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
}

void print_udp_packet(const u_char* packet, int size) {
	unsigned short iphdrlen = 0, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct udphdr *udph = NULL;
	int header_size = 0;

	iph = (struct iphdr *)(packet +  sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;

	udph = (struct udphdr*)(packet + iphdrlen + sizeof(struct ethhdr));

	header_size = sizeof(struct ethhdr) + iphdrlen + sizeof udph;

	check_sum_back = udph->check;
	compute_udp_checksum(iph, udph);

	printf("\n***********************UDP Packet*************************\n");

	print_ip_header(packet, size);

	printf("\nUDP Header\n");
	printf("   |-Source Port      : %d\n", ntohs(udph->source));
	printf("   |-Destination Port : %d\n", ntohs(udph->dest));
	printf("   |-UDP Length       : %d\n", ntohs(udph->len));
	printf("   |-UDP Checksum     : %d (verif: %d) [%s]\n", ntohs(check_sum_back), htons(udph->check), (check_sum_back == udph->check) ? "valid" : "invalid");

	printf("\n");
	printf("IP Header\n");
	print_data(packet, iphdrlen);

	printf("UDP Header\n");
	print_data(packet+iphdrlen, sizeof udph);

	printf("Data Payload\n");

	print_data(packet + header_size, size - header_size);

	printf("\n###########################################################");
}

void print_data(const u_char* data, int size) {
	int i = 0, j = 0;

	for(i = 0; i < size; i++) {
		if(i != 0 && i%16 == 0) {
			printf("         ");

			for(j=i-16 ; j<i ; j++) {
				if(data[j]>=32 && data[j]<=128)
					printf("%c", (unsigned char)data[j]);
				else
					printf(".");
			}

			printf("\n");
		}

		if(i%16 == 0)
			printf("   ");

		printf(" %02X",(unsigned int)data[i]);

		if(i == size-1) {
			for(j = 0; j < 15 - i%16; j++) {
				printf("   ");
			}

			printf("         ");

			for(j = i - i%16; j <= i ; j++) {
				if(data[j]>=32 && data[j]<=128) {
					printf("%c",(unsigned char)data[j]);
				}
				else {
					printf(".");
				}
			}

			printf( "\n" );
		}
	}
}
