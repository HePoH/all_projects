#include "../include/core.h"

void print_usage(char **argv) {
    printf("usage syntax: ./%s -c [count] -t [timeout] -D [debug] -d [destination address] -s [source address]\n", argv[0]);
    printf(" -- destination address must be provided\n");
    printf(" -- source address is optional\n");
}

int parse_argvs(int argc, char **argv, ping_sets_t *ping_sets) {
    uint8_t opt = 0, ipv4_addr[INET_ADDRSTRLEN], res = 0;

    while ((opt = getopt (argc, argv, "c:t:Dd:s:")) != -1) {
        switch (opt) {
            case 'c':
                res = sscanf(optarg, "%d", (int*)&(ping_sets->reqs_count));
                if (res != 1)
                    return -1;
                break;
            case 't':
                res = sscanf(optarg, "%d", (int*)&(ping_sets->reqs_timeout));
                if (res != 1)
                    return -1;
                break;
            case 'D':
                ping_sets->debug_opt = 1;
                break;
            case 'd':
                res = sscanf(optarg, "%[^:]", ipv4_addr);
                if (res != 1)
                    return -1;

                res = inet_pton(AF_INET, (char*)ipv4_addr, (void*)&(ping_sets->dst_addr.sin_addr));
                if (res == -1) {
                    perror("inet_pton() failed");
                    return -1;
                }

                res = get_self_ip(&(ping_sets->src_addr.sin_addr), DEF_IF_NAME);
                if (res == -1) {
                    printf("get_self_ip() failed\n");
                    return -1;
                }

                break;
            case 's':
                res = sscanf(optarg, "%[^:]", ipv4_addr);
                if (res != 1)
                    return -1;

                res = inet_pton(AF_INET, (char*)ipv4_addr, (void*)&(ping_sets->src_addr.sin_addr));
                if (res == -1) {
                    perror("inet_pton() failed");
                    return -1;
                }
                break;
            case '?':
                printf("%s: too few or invalid arguments syntax to initialize\n", argv[0]);
                print_usage(argv);

                return -1;

            default:
                return 0;
        }
    }
}

int get_self_ip(struct in_addr *sin_addr, char *if_name) {
    uint32_t sock = 0, res = 0;
    struct ifreq ifr;
    struct sockaddr_in *p_addr = NULL;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket() failed");

        close(sock);
        return -1;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);

    res = ioctl(sock, SIOCGIFADDR, &ifr);
    close(sock);
    if (res == 0) {
        p_addr = ((struct sockaddr_in*)&ifr.ifr_addr);
        memcpy(sin_addr, &(p_addr->sin_addr), sizeof(struct in_addr));
        return 0;
    }
    else
        return -1;
}

void print_if_addr() {
	struct ifaddrs *ifaddr = NULL, *ifa = NULL;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
			    return;
			}
			printf("<Interface>: %s \t <Address> %s\n", ifa->ifa_name, host);
		}
	}
}

double get_diff_time(struct timeval t0, struct timeval t1) {
    return fabs((t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f);
}

void sig_hndl(int sig_num) {
    printf("\n--- ping statistics ---\n");
    printf("%d packets transmitted, %d received, %1.f%% packet loss\n",
            (int)stat_pckt_transm,
            (int)stat_pckt_recive,
            (double)(100 - stat_pckt_recive/stat_pckt_transm*100));

    exit(EXIT_SUCCESS);
    return;
}

void init_signal() {
    int res = 0;
    struct sigaction sig_act;

    sig_act.sa_handler = sig_hndl;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = 0;

    res = sigaction(SIGINT, &sig_act, NULL);
    if (res == -1) {
        printf("sigaction() failed\n");
        exit(EXIT_FAILURE);
    }
}

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
	uint16_t /*iphdrlen = 0,*/ check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct sockaddr_in source, dest;

	iph = (struct iphdr *)packet;
	/*iphdrlen = iph->ihl*4;*/

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;

	check_sum_back = iph->check;
	compute_ip_checksum(iph);

	printf("\nIP Header\n");
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
	uint16_t iphdrlen, check_sum_back = 0;
	struct iphdr *iph = NULL;
	struct icmphdr *icmph = NULL;
	uint32_t header_size = 0;

	iph = (struct iphdr *)packet;
	iphdrlen = iph->ihl*4;

	icmph = (struct icmphdr *)(packet + iphdrlen);
	header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;

	check_sum_back = icmph->checksum;
	compute_icmp_checksum(icmph);

	printf("\n*********************** ICMP Packet *************************\n");

	print_ip_header(packet, size);
	printf("\n");

	printf("ICMP Header\n");
	printf("   |-Type     : %d  ", (unsigned int)(icmph->type));

	switch((unsigned int)(icmph->type)) {
		case ICMP_ECHOREPLY:
				printf("(Echo Reply)\n");
				break;
		case ICMP_DEST_UNREACH:
				printf("(Destination Unreachable)\n");
				break;
		case ICMP_SOURCE_QUENCH:
				printf("(Source Quench)\n");
				break;
		case ICMP_REDIRECT:
				printf("(Redirect (change route))\n");
				break;
		case ICMP_ECHO:
				printf("(Echo Request)\n");
				break;
		case ICMP_TIME_EXCEEDED:
				printf("(Time Exceeded)\n");
				break;
		case ICMP_PARAMETERPROB:
				printf("(Parameter Problem)\n");
				break;
		case ICMP_TIMESTAMP:
				printf("(Timestamp Request)\n");
				break;
		case ICMP_TIMESTAMPREPLY:
				printf("(Timestamp Reply)\n");
				break;
		case ICMP_INFO_REQUEST:
				printf("(Information Request)\n");
				break;
		case ICMP_INFO_REPLY:
				printf("(Information Reply)\n");
				break;
		case ICMP_ADDRESS:
				printf("(Address Mask Request)\n");
				break;
		case ICMP_ADDRESSREPLY:
				printf("(Address Mask Reply)\n");
				break;
		default:
				printf("\n");
				break;
	}

	printf("   |-Code     : %d\n", (unsigned int)(icmph->code));
	printf("   |-Checksum : %d (verif: %d) [%s]\n", ntohs(check_sum_back), ntohs(icmph->checksum), (check_sum_back == icmph->checksum) ? "valid" : "invalid");
	printf("   |-ID       : %d\n", ntohs(icmph->un.echo.id));
	printf("   |-Sequence : %d\n", ntohs(icmph->un.echo.sequence));
	printf("\n");

	printf("IP header data\n");
	print_data(packet, iphdrlen);

	printf("UDP header data\n");
	print_data(packet + iphdrlen, sizeof icmph);

	printf("Data payload\n");
	print_data(packet + header_size, size - header_size);

	printf("\n###########################################################");
}

void print_data(const u_char* data, int size) {
	uint8_t i = 0, j = 0;

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

			for(j = i - i%16; j <= i; j++) {
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

void* icmp_reqs_hndl(void* args) {
    uint32_t i = 0, seq_num = 0;
    ping_sets_t *ping_sets = NULL;
    struct iphdr* reqs_iph = NULL;
    struct icmphdr* reqs_icmph = NULL;
    uint8_t reqs_pckt[MAX_PACKET_SIZE];
    socklen_t sock_len = 0;
    ssize_t res = 0;

    if (args == NULL) {
        perror("ping settings argument is null");
        exit(EXIT_FAILURE);
    }
    ping_sets = args;

    sock_len = sizeof(struct sockaddr_in);

    memset(reqs_pckt, 0, MAX_PACKET_SIZE);

    reqs_iph = (struct iphdr*)reqs_pckt;
    reqs_icmph = (struct icmphdr*)(reqs_pckt + sizeof(struct iphdr));

    reqs_iph->ihl = 5;
    reqs_iph->version = 4;
    reqs_iph->tos = 0;
    reqs_iph->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
    reqs_iph->id = htonl(random());
    reqs_iph->frag_off = 0;
    reqs_iph->ttl = 255;
    reqs_iph->protocol = IPPROTO_ICMP;
    reqs_iph->check = 0;
    reqs_iph->saddr = ping_sets->src_addr.sin_addr.s_addr;
    reqs_iph->daddr = ping_sets->dst_addr.sin_addr.s_addr;

    reqs_icmph->type = ICMP_ECHO;
    reqs_icmph->code = 0;
    reqs_icmph->checksum = 0;
    reqs_icmph->un.echo.id = htons(ping_sets->reqs_id);
    reqs_icmph->un.echo.sequence = htons(seq_num);

    compute_icmp_checksum(reqs_icmph);
    compute_ip_checksum(reqs_iph);

    for(i = 0; i < ping_sets->reqs_count; i++) {
        gettimeofday(&(ping_sets->diff_time[seq_num].reqs_time), NULL);
        res = sendto(ping_sets->sock, reqs_pckt, reqs_iph->tot_len, 0, (struct sockaddr*)&(ping_sets->dst_addr), sock_len);
        if (res == -1) {
            perror("sendto() failed");
            exit(EXIT_FAILURE);
        }

        stat_pckt_transm++;
        seq_num++;
        reqs_icmph->checksum = 0;
        reqs_icmph->un.echo.sequence = htons(seq_num);

        compute_icmp_checksum(reqs_icmph);
        sleep(1);
    }

    return NULL;
}

void *icmp_repl_hndl(void* args) {
    uint32_t seq_num = 0, reqs_count = 0;
    ping_sets_t *ping_sets = NULL;
    uint16_t repl_iphlen = 0;
    struct sockaddr_in dst_addr;
    struct hostent* dst_hst = NULL;
    struct iphdr *repl_iph = NULL;
    struct icmphdr *repl_icmph = NULL;
    uint8_t repl_pckt[MAX_PACKET_SIZE], ipv4_addr[INET_ADDRSTRLEN];
    socklen_t sock_len = 0;
    ssize_t res = 0;

    if (args == NULL) {
        perror("ping settings argument is null");
        exit(EXIT_FAILURE);
    }
    ping_sets = args;

    inet_ntop(AF_INET, (const void*)&(ping_sets->dst_addr.sin_addr), (char*)ipv4_addr, INET_ADDRSTRLEN);
    dst_hst = gethostbyaddr((char *)&(ping_sets->dst_addr.sin_addr), 4, AF_INET);

    memset(repl_pckt, 0, MAX_PACKET_SIZE);
    printf("\n");

    while(1) {
        memset(repl_pckt, 0, MAX_PACKET_SIZE);
        sock_len = sizeof(struct sockaddr_in);

        res = recvfrom(ping_sets->sock, repl_pckt, MAX_PACKET_SIZE, 0, (struct sockaddr *)&dst_addr, (socklen_t *)&sock_len);
        if (res == -1) {
            perror("recvfrom() failed");
            exit(EXIT_FAILURE);
        }

        repl_iph = (struct iphdr*)repl_pckt;
        repl_iphlen = repl_iph->ihl*4;
        repl_icmph = (struct icmphdr*)(repl_pckt + repl_iphlen);

        if (ntohs(repl_icmph->un.echo.id) != ping_sets->reqs_id)
            continue;

        switch(repl_icmph->type) {
            case ICMP_ECHOREPLY:
                /*if (ping_sets->debug_opt)
                    print_icmp_packet((u_char*)repl_pckt, sizeof(repl_pckt));*/

                seq_num = ntohs(repl_icmph->un.echo.sequence);
                gettimeofday(&(ping_sets->diff_time[seq_num].repl_time), NULL);
                printf("%d bytes from %s (ip: %s): icmp_seq=%d ttl=%d time=%1.2f ms\n",
                        res,
                        ((dst_hst != NULL) ? dst_hst->h_name : ""),
                        ipv4_addr,
                        ntohs(repl_icmph->un.echo.sequence),
                        repl_iph->ttl,
                        get_diff_time(ping_sets->diff_time[seq_num].repl_time, ping_sets->diff_time[seq_num].reqs_time));

                stat_pckt_recive++;
                reqs_count++;
                if (ping_sets->reqs_count == reqs_count)
                    pthread_exit(NULL);
                break;

            case ICMP_DEST_UNREACH:
                printf("destination unreachable\n");
                exit(EXIT_FAILURE);
                break;

            case ICMP_TIME_EXCEEDED:
                printf("time exceed\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    return NULL;
}
