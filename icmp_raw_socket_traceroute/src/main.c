#include "../include/core.h"

int main() {
	int sd = 0, epd = 0, ttl = 4, seq = 0, on = 1, rtn = 0;
	pid_t pid = 0;
	unsigned short reqs_iphlen = 0, repl_iphlen = 0;
	struct sockaddr_in dest_addr, repl_source;
	struct hostent* dest_hst = NULL;
    struct epoll_event ev, *events = NULL;
	struct iphdr *reqs_iph = NULL, *repl_iph = NULL;
	struct icmphdr *reqs_icmph = NULL, *repl_icmph = NULL;
	char reqs_pckt[MAX_PACKET_SIZE], repl_pckt[MAX_PACKET_SIZE];
	socklen_t socket_len = 0;
	ssize_t bts = 0;

	pid = getpid();

	socket_len = sizeof(struct sockaddr_in);

	memset(&dest_addr, 0, socket_len);
	memset(reqs_pckt, 0, MAX_PACKET_SIZE);
	memset(repl_pckt, 0, MAX_PACKET_SIZE);
    memset(&ev, 0, sizeof(struct epoll_event));

    events = calloc(1, sizeof(struct epoll_event));
    if (events == NULL) {
        perror("calloc(events)");
        exit(EXIT_FAILURE);
    }

    epd = epoll_create(1);
    if (epd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = sd;

    rtn = epoll_ctl(epd, EPOLL_CTL_ADD, sd, &ev);
    if (rtn == -1) {
        perror("epoll_ctl(sd)");
        exit(EXIT_FAILURE);
    }

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
	reqs_iph->ttl = ttl;
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

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sd == -1) {
		perror("socket");

		close(sd);
		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if (rtn == -1) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/*dest_hst = gethostbyaddr((char *)&dest_addr.sin_addr.s_addr, 4, AF_INET);
	printf("Client: icmp echo server %s:%d (hs: %s)\n", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port),
	((dest_hst != NULL) ? dest_hst->h_name : ""));*/

	while(1) {
		dest_hst = gethostbyaddr((char *)&dest_addr.sin_addr.s_addr, 4, AF_INET);

		bts = sendto(sd, reqs_pckt, reqs_iph->tot_len, 0, (struct sockaddr*)&dest_addr, socket_len);
		if (bts == -1) {
			perror("sendto");

			close(sd);
			exit(EXIT_FAILURE);
		}

		printf("Sent %d byte packet to %s (hs: %s)\n", bts, DEST_IP, ((dest_hst != NULL) ? dest_hst->h_name : ""));

		memset(repl_pckt, 0, MAX_PACKET_SIZE);
		memset(&repl_source, 0, sizeof(struct sockaddr_in));

        rtn = epoll_wait(epd, events, 1, 3000);
        if (rtn == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        else
            if (rtn == 0) {
                perror("time exceeded");

                reqs_iph->check = 0;
    		    reqs_iph->ttl = ++ttl;

	    	    reqs_icmph->checksum = 0;
		        reqs_icmph->un.echo.sequence = htons(++seq);

		        compute_icmp_checksum(reqs_icmph);
	    	    compute_ip_checksum(reqs_iph);

                continue;
            }

		bts = recvfrom(sd, repl_pckt, MAX_PACKET_SIZE, 0, NULL, NULL);
		if (bts == -1) {
			perror("recvfrom");

			close(sd);
			exit(EXIT_FAILURE);
		}

		repl_iph = (struct iphdr*)repl_pckt;
		repl_iphlen = repl_iph->ihl*4;
		repl_icmph = (struct icmphdr*)(repl_pckt + repl_iphlen);

		repl_source.sin_addr.s_addr = repl_iph->saddr;
		dest_hst = gethostbyaddr((char *)&repl_source.sin_addr.s_addr, 4, AF_INET);
		print_icmp_packet((u_char*)repl_pckt, sizeof(repl_pckt));
		printf("\nReceived %d byte reply from %s (hs: %s)\n", bts, inet_ntoa(repl_source.sin_addr), ((dest_hst != NULL) ? dest_hst->h_name : ""));

		reqs_iph->check = 0;
		reqs_iph->ttl = ++ttl;

		reqs_icmph->checksum = 0;
		reqs_icmph->un.echo.sequence = htons(++seq);

		compute_icmp_checksum(reqs_icmph);
		compute_ip_checksum(reqs_iph);
	}

	exit(EXIT_SUCCESS);
}
