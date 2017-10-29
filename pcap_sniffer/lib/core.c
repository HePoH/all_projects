#include "../include/core.h"

void print_usage_help() {
    printf("usage syntax: ./sniffer [-h/f/if] -if [interface] -f [filter] -s [search string] -l [packet number limit]\n");
}

void print_filter_help() {
    int fd = 0;
	ssize_t rb = 0;
	char buffer[MAX_BUF_SIZE];

	fd = open(HELP_FILTER_FILE, O_RDONLY);
	if (fd < 0) {
		printf("open() failed: cannot open file\n");
		exit (1);
	}	

    while ((rb = read(fd, buffer, MAX_BUF_SIZE)) > 0) {
		buffer[rb] = 0;
        printf("%s", buffer);
	}
}

char *get_ip_str(const struct sockaddr *sa, char *str, size_t max_len) {
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    str, max_len);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    str, max_len);
            break;

        default:
            strncpy(str, "Unknown AF", max_len);
    }

    return str;
}

void list_net_devs() {
	int res = 0;
	pcap_if_t *net_devs_list = NULL, *curr_dev = NULL;
	pcap_addr_t* dev_addr = NULL;
	char ipv4_buff[INET_ADDRSTRLEN], ipv6_buff[INET6_ADDRSTRLEN], pcap_err_buf[PCAP_ERRBUF_SIZE];

	res = pcap_findalldevs(&net_devs_list, pcap_err_buf);
	if (res == -1) {
        printf("pcap_findalldevs failed: couldn't get all network devices: %s\n",
                pcap_err_buf);
        exit(EXIT_FAILURE);
    }

	printf("+----------+--------------------------------------------------+------------------------------------------------+\n");
	printf("| %-108s |\n", "                                       Available network devices list");
	printf("+----------+--------------------------------------------------+------------------------------------------------+\n");
	printf("| %-8s | %-48s | %-46s |\n", "Name", "Description", "Network address");
	printf("+----------+--------------------------------------------------+------------------------------------------------+\n");

	curr_dev = net_devs_list;
	while(curr_dev) {
		printf("| %-8s | %-48s |", curr_dev->name, (curr_dev->description ? curr_dev->description : "(no description)"));

		dev_addr = curr_dev->addresses;
        if (dev_addr == NULL)
		    printf(" %-46s |\n", "(no address)");
	
        while(dev_addr)
            if (dev_addr->addr->sa_family == AF_INET) {
		        printf(" %-46s |\n", get_ip_str((struct sockaddr*)dev_addr->addr, ipv4_buff, INET_ADDRSTRLEN));
                dev_addr = dev_addr->next;
                break;
            }
            else
                if (dev_addr->addr->sa_family == AF_INET6) {
		            printf(" %-46s |\n", get_ip_str((struct sockaddr6*)dev_addr->addr, ipv6_buff, INET6_ADDRSTRLEN));
                    dev_addr = dev_addr->next;
                    break;
                }
                else
                    dev_addr = dev_addr->next;

        while(dev_addr) {
			switch(dev_addr->addr->sa_family) {
				case AF_INET:
		            printf("| %-8s | %-48s | %-46s |\n", " ", " ", get_ip_str((struct sockaddr*)dev_addr->addr, ipv4_buff, INET_ADDRSTRLEN));
					break;

				case AF_INET6:
		            printf("| %-8s | %-48s | %-46s |\n", " ", " ", get_ip_str((struct sockaddr6*)dev_addr->addr, ipv6_buff, INET6_ADDRSTRLEN));
					break;

				default:
					break;
			}

			dev_addr = dev_addr->next;
		}
        
	    printf("+----------+--------------------------------------------------+------------------------------------------------+\n");
		curr_dev = curr_dev->next;
	}

	if (net_devs_list)
		pcap_freealldevs(net_devs_list);
}

void compute_ip_checksum(struct iphdr* iph) {
	iph->check = 0;
	iph->check = compute_checksum((unsigned short*)iph, iph->ihl<<2);
}

void compute_tcp_checksum(struct iphdr* iph, struct tcphdr* tcph) {
	uint16_t iphlen = 0, tcpplen = 0;
	uint8_t *block = NULL;
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
	uint16_t iphlen = 0, udpplen = 0;
	uint8_t *block = NULL;
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

void compute_icmp_checksum(struct icmphdr* icmph) {
	icmph->checksum = 0;
	icmph->checksum = compute_checksum((unsigned short*)icmph, sizeof(struct icmphdr));
}

uint16_t compute_checksum(uint16_t* addr, uint32_t count) {
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
	return ((uint16_t)sum);
}

void pckt_hndl(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	int res = 0, size = 0;
	struct iphdr *iph = NULL;

	size = header->len;
	iph = (struct iphdr*)(packet + sizeof(struct ethhdr));

	++total;

	switch(iph->protocol) {
		/* ICMP Protocol */
		case 1:
			++icmp;
			print_icmp_packet(packet, size);
			break;

		/* IGMP Protocol */
		case 2:
			++igmp;
			break;

		/* TCP Protocol */
		case 6:
			++tcp;
            if (args == NULL)
			    print_tcp_packet(packet, size);
            else {
                res = check_data(packet, size, iph->protocol, args);
                if (res != -1)
			        print_tcp_packet(packet, size);
            }
			break;

		/* UDP Protocol */
		case 17:
			++udp;
            if (args == NULL)
			    print_udp_packet(packet, size);
            else {
                res = check_data(packet, size, iph->protocol, args);
                if (res != -1)
			        print_udp_packet(packet, size);
            }
			break;

		/* Some other protocol like ARP etc. */
		default:
	        printf("\n***********************Other protocol, skip the packet*************************\n");
			++others;
			break;
	}

    printf("\nStatistic:\n");
	printf("   |-TCP : %d\n", tcp);
	printf("   |-UDP: %d\n", udp);
	printf("   |-ICMP: %d\n", icmp);
	printf("   |-OTH: %d\n", others);
	printf("   |-TOT: %d\n", total);
}

int check_data(const u_char *packet, int size, int proto, char *search_str) {
    uint8_t *res = NULL, *data = NULL;
	uint16_t ip_hdr_len = 0;
	struct iphdr *iph = NULL;
    struct tcphdr *tcph = NULL;
	uint32_t i = 0, k = 0, search_str_len = 0, hdr_size = 0;

	iph = (struct iphdr *)(packet +  sizeof(struct ethhdr));
	ip_hdr_len = iph->ihl*4;

	switch(proto) {
	    /* TCP Protocol */
		case 6:
			tcph = (struct tcphdr*)(packet + ip_hdr_len + sizeof(struct ethhdr));
        	hdr_size =  sizeof(struct ethhdr) + ip_hdr_len + tcph->doff*4;
			break;

		/* UDP Protocol */
		case 17:
	        hdr_size = sizeof(struct ethhdr) + ip_hdr_len + sizeof(struct udphdr);
			break;

		/* Some other protocol like ARP etc. */
		default:
            return -1;
			break;
	}

    data = packet + hdr_size;
    search_str_len = strlen(search_str);
    for(i = 0; i < size - hdr_size; i++){
        if (k == search_str_len)
            return 0;

        for(k = 0; k < search_str_len; k++)
            if (data[i+k] != search_str[k])
                break;
    }
    
    return -1;
}

void print_ethernet_header(const u_char* frame, int size) {
	struct ethhdr *eth = NULL;
	eth = (struct ethhdr *)frame;

	printf("\n");
	printf("Ethernet Header\n");
	printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
            eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], 
            eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
	printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", 
            eth->h_source[0], eth->h_source[1], eth->h_source[2], 
            eth->h_source[3], eth->h_source[4], eth->h_source[5]);
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
	printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n", 
            (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
	printf("   |-Type Of Service   : %d\n", (unsigned int)iph->tos);
	printf("   |-IP Total Length   : %d Bytes(Size of Packet)\n", 
            ntohs(iph->tot_len));
	printf("   |-Identification    : %d\n", ntohs(iph->id));
	/*printf("   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);*/
	/*printf("   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);*/
	/*printf("   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);*/
	printf("   |-TTL               : %d\n", (unsigned int)iph->ttl);
	printf("   |-Protocol          : %d\n", (unsigned int)iph->protocol);
	printf("   |-Checksum          : %d (verif: %d) [%s]\n", 
            ntohs(check_sum_back), ntohs(iph->check), 
            (check_sum_back == iph->check) ? "valid" : "invalid");
	printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
	printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(const u_char* packet, int size) {
	unsigned short iphdrlen = 0, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	int header_size = 0;

	iph = (struct iphdr *)(packet  + sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;
	tcph =  (struct tcphdr*)(packet + iphdrlen + sizeof(struct ethhdr));
	header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

	check_sum_back = tcph->check;
	compute_tcp_checksum(iph, tcph);

	printf("\n***********************TCP Packet*************************\n");

	print_ip_header(packet, size);

	printf("\n");
	printf("TCP Header\n");
	printf("   |-Source Port          : %u\n", ntohs(tcph->source));
	printf("   |-Destination Port     : %u\n", ntohs(tcph->dest));
	printf("   |-Sequence Number      : %u\n", ntohl(tcph->seq));
	printf("   |-Acknowledge Number   : %u\n", ntohl(tcph->ack_seq));
	printf("   |-Header Length        : %d DWORDS or %d BYTES\n", (unsigned int)tcph->doff, (unsigned int)tcph->doff*4);
	/*printf("   |-CWR Flag : %d\n", (unsigned int)tcph->cwr);*/
	/*printf("   |-ECN Flag : %d\n", (unsigned int)tcph->ece);*/
	printf("   |-Urgent Flag          : %d\n", (unsigned int)tcph->urg);
	printf("   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
	printf("   |-Push Flag            : %d\n", (unsigned int)tcph->psh);
	printf("   |-Reset Flag           : %d\n", (unsigned int)tcph->rst);
	printf("   |-Synchronise Flag     : %d\n", (unsigned int)tcph->syn);
	printf("   |-Finish Flag          : %d\n", (unsigned int)tcph->fin);
	printf("   |-Window               : %d\n", ntohs(tcph->window));
	printf("   |-Checksum             : %d (verif: %d) [%s]\n", 
            ntohs(check_sum_back), htons(tcph->check), 
            (check_sum_back == tcph->check) ? "valid" : "invalid");
	printf("   |-Urgent Pointer       : %d\n", tcph->urg_ptr);
	printf("\n");
	printf("                        DATA Dump                         ");
	printf("\n");

	printf("IP Header\n");
	print_data(packet, iphdrlen);

	printf("TCP Header\n");
	print_data(packet+iphdrlen, tcph->doff*4);

	printf("Data Payload\n");
	print_data(packet + header_size , size - header_size);

	printf("\n###########################################################\n");
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
	printf("   |-UDP Checksum     : %d (verif: %d) [%s]\n", 
            ntohs(check_sum_back), htons(udph->check), 
            (check_sum_back == udph->check) ? "valid" : "invalid");

	printf("\n");
	printf("IP Header\n");
	print_data(packet, iphdrlen);

	printf("UDP Header\n");
	print_data(packet+iphdrlen, sizeof udph);

	printf("Data Payload\n");
	print_data(packet + header_size, size - header_size);

	printf("\n###########################################################\n");
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
	printf("   |-Checksum : %d (verif: %d) [%s]\n", ntohs(check_sum_back), 
            ntohs(icmph->checksum), (check_sum_back == icmph->checksum) ? "valid" : "invalid");
	printf("   |-ID       : %d\n", ntohs(icmph->un.echo.id));
	printf("   |-Sequence : %d\n", ntohs(icmph->un.echo.sequence));
	printf("\n");

	printf("IP Header\n");
	print_data(packet, iphdrlen);

	printf("UDP Header\n");
	print_data(packet + iphdrlen, sizeof icmph);

	printf("Data Payload\n");
	print_data(packet + header_size, size - header_size);

	printf("\n###########################################################\n");
}

void print_data(const u_char* data, uint32_t size) {
	uint32_t i = 0, j = 0;

	for(i = 0; i < size; i++) {
		if(i != 0 && i % 16 == 0) {
			printf("         ");

			for(j = i-16 ; j < i; j++) {
				if(data[j] >= 32 && data[j] <= 128)
					printf("%c", (uint8_t)data[j]);
				else
					printf(".");
			}

			printf("\n");
		}

		if(i % 16 == 0)
			printf("   ");

		printf(" %02X", (uint32_t)data[i]);

		if(i == size-1) {
			for(j = 0; j < 15 - i % 16; j++) {
				printf("   ");
			}

			printf("         ");

			for(j = i - i % 16; j <= i; j++) {
				if(data[j] >= 32 && data[j] <= 128) {
					printf("%c", (uint8_t)data[j]);
				}
				else {
					printf(".");
				}
			}

			printf( "\n" );
		}
	}
}
