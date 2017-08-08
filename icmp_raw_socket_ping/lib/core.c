#include "../include/core.h"

void compute_ip_checksum(struct iphdr* iph) {
	iph->check = 0;
	iph->check = compute_checksum((unsigned short*)iph, iph->ihl<<2);
}

void compute_icmp_checksum(struct icmphdr* icmph) {
	icmph->checksum = 0;
	icmph->checksum = compute_checksum((unsigned short*)icmph, sizeof(struct icmphdr));
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
	unsigned short iphdrlen, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct icmphdr *icmph = NULL;
	int header_size = 0;

	iph = (struct iphdr *)packet;
	iphdrlen = iph->ihl*4;

	icmph = (struct icmphdr *)(packet + iphdrlen);
	header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

	check_sum_back = icmph->checksum;
	compute_icmp_checksum(icmph);

	printf("\n***********************ICMP Packet*************************\n");

	print_ip_header(packet, size);

	printf("\n");

	printf("ICMP Header\n");
	printf("   |-Type : %d", (unsigned int)(icmph->type));

	switch((unsigned int)(icmph->type)) {
		case ICMP_ECHOREPLY:
				printf("  (Echo Reply)\n");
				break;

		case ICMP_DEST_UNREACH:
				printf("  (Destination Unreachable)\n");
				break;

		case ICMP_SOURCE_QUENCH:
				printf("  (Source Quench)\n");
				break;

		case ICMP_REDIRECT:
				printf("  (Redirect (change route))\n");
				break;

		case ICMP_ECHO:
				printf("  (Echo Request)\n");
				break;

		case ICMP_TIME_EXCEEDED:
				printf("  (Time Exceeded)\n");
				break;

		case ICMP_PARAMETERPROB:
				printf("  (Parameter Problem)\n");
				break;

		case ICMP_TIMESTAMP:
				printf("  (Timestamp Request)\n");
				break;

		case ICMP_TIMESTAMPREPLY:
				printf("  (Timestamp Reply)\n");
				break;

		case ICMP_INFO_REQUEST:
				printf("  (Information Request)\n");
				break;

		case ICMP_INFO_REPLY:
				printf("  (Information Reply)\n");
				break;

		case ICMP_ADDRESS:
				printf("  (Address Mask Request)\n");
				break;

		case ICMP_ADDRESSREPLY:
				printf("  (Address Mask Reply)\n");
				break;
		default:
				printf("\n");
				break;
	}

	printf("   |-Code : %d\n", (unsigned int)(icmph->code));
	printf("   |-Checksum : %d (verif: %d) [%s]\n", ntohs(check_sum_back), ntohs(icmph->checksum), (check_sum_back == icmph->checksum) ? "valid" : "invalid");
	printf("   |-ID       : %d\n", ntohs(icmph->un.echo.id));
	printf("   |-Sequence : %d\n", ntohs(icmph->un.echo.sequence));
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

void* reqs_hndl(void* args) {
    int sd = 0, seq = 0;
    pid_t pid = 0;
    unsigned short reqs_iphlen = 0;
    struct sockaddr_in dest_addr;
    struct iphdr* reqs_iph = NULL;
    struct icmphdr* reqs_icmph = NULL;
    char reqs_pckt[MAX_PACKET_SIZE];
    socklen_t socket_len = 0;
    ssize_t bts = 0;

    if (args == NULL) {
        perror("args is NULL");
        exit(EXIT_FAILURE);
    }

    sd = *((int*)args);
    pid = getpid();

    socket_len = sizeof(struct sockaddr_in);

    memset(&dest_addr, 0, socket_len);
    memset(reqs_pckt, 0, MAX_PACKET_SIZE);

    reqs_iph = (struct iphdr*)reqs_pckt;
    reqs_icmph = (struct icmphdr*)(reqs_pckt + sizeof(struct iphdr));

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    reqs_iph->ihl = 5;
    reqs_iph->version = 4;
    reqs_iph->tos = 0;
    reqs_iph->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
    reqs_iph->id = htonl(random());
    reqs_iph->frag_off = 0;
    reqs_iph->ttl = 255;
    reqs_iph->protocol = IPPROTO_ICMP;
    reqs_iph->check = 0;
    reqs_iph->saddr = inet_addr(SOURCE_IP);
    reqs_iph->daddr = inet_addr(DEST_IP);

    reqs_icmph->type = ICMP_ECHO;
    reqs_icmph->code = 0;
    reqs_icmph->checksum = 0;
    reqs_icmph->un.echo.id = pid;
    reqs_icmph->un.echo.sequence = htons(seq);

    compute_icmp_checksum(reqs_icmph);
    compute_ip_checksum(reqs_iph);

    while(1) {
        bts = sendto(sd, reqs_pckt, reqs_iph->tot_len, 0, (struct sockaddr*)&dest_addr, socket_len);
        if (bts == -1) {
            perror("sendto");

            close(sd);
            exit(EXIT_FAILURE);
        }

        printf("\nSent %d byte packet to %s icmp_seq = %d\n", (int)bts, DEST_IP, seq);

        reqs_icmph->checksum = 0;
        reqs_icmph->un.echo.sequence = htons(++seq);
        compute_icmp_checksum(reqs_icmph);

        sleep(1);
    }
}

void* repl_hndl(void* args) {
    int sd = 0;
    pid_t pid = 0;
    unsigned short repl_iphlen = 0;
    struct iphdr *repl_iph = NULL;
    struct icmphdr *repl_icmph = NULL;
    char repl_pckt[MAX_PACKET_SIZE];
    ssize_t bts = 0;

    if (args == NULL) {
        perror("args is NULL");
        exit(EXIT_FAILURE);
    }

    sd = *((int*)args);
    pid = getpid();
    memset(repl_pckt, 0, MAX_PACKET_SIZE);

    while(1) {
        memset(repl_pckt, 0, MAX_PACKET_SIZE);

        bts = recvfrom(sd, repl_pckt, MAX_PACKET_SIZE, 0, NULL, NULL);
        if (bts == -1) {
            perror("recvfrom");

            close(sd);
            exit(EXIT_FAILURE);
        }

        repl_iph = (struct iphdr*)repl_pckt;
        repl_iphlen = repl_iph->ihl*4;
        repl_icmph = (struct icmphdr*)(repl_pckt + repl_iphlen);

        /*print_icmp_packet((u_char*)repl_pckt, sizeof(repl_pckt));*/
        printf("Received %d byte reply from %s icmp_seq = %d\n", (int)bts, DEST_IP, ntohs(repl_icmph->un.echo.sequence));
    }
}
