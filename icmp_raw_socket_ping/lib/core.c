#include "../include/core.h"

void compute_ip_checksum(struct iphdr* iph) {
	iph->check = 0;
	iph->check = compute_checksum((unsigned short*)iph, iph->ihl<<2);
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

void print_ip_header(const u_char* packet, int size) {
	unsigned short iphdrlen = 0, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct sockaddr_in source, dest;

	iph = (struct iphdr *)packet;
	iphdrlen = iph->ihl*4;

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

void print_icmp_packet(const u_char* packet, int size) {
	unsigned short iphdrlen;
	struct iphdr *iph = NULL;
	struct icmphdr *icmph = NULL;
	int header_size = 0;

	iph = (struct iphdr *)packet;
	iphdrlen = iph->ihl*4;

	icmph = (struct icmphdr *)(packet + iphdrlen);

	header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

	printf("\n***********************ICMP Packet*************************\n");

	print_ip_header(packet, size);

	printf("\n");

	printf("ICMP Header\n");
	printf("   |-Type : %d", (unsigned int)(icmph->type));

	if((unsigned int)(icmph->type) == 11) {
		printf("  (TTL Expired)\n");
	}
	else
		if((unsigned int)(icmph->type) == ICMP_ECHOREPLY) {
			printf("  (ICMP Echo Reply)\n");
		}

	printf("   |-Code : %d\n", (unsigned int)(icmph->code));
	printf("   |-Checksum : %d\n", ntohs(icmph->checksum));
	/*printf("   |-ID       : %d\n",ntohs(icmph->id));*/
	/*printf("   |-Sequence : %d\n",ntohs(icmph->sequence));*/
	printf("\n");

	printf("IP Header\n");
	print_data(packet, iphdrlen);

	printf("UDP Header\n");
	print_data(packet + iphdrlen, sizeof icmph);

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
