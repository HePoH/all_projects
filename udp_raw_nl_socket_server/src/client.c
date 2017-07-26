#include "../include/client_core.h"

int main() {
	int sd_cln = 0, header_size = 0, on = 1, rtn = 0;
	unsigned short iphlen = 0, udpplen = 0;
	struct sockaddr_in srv_addr;
	struct hostent* srv_hst = NULL;
	struct iphdr *iph = NULL;
	struct udphdr* udph = NULL;
	char dgram[MAX_DGRAM_SIZE], *data = NULL;
	socklen_t socket_len = 0;
	ssize_t bts = 0;

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(dgram, 0, MAX_DGRAM_SIZE);

	iph = (struct iphdr*)dgram;
	iphlen = sizeof(struct iphdr);
	udph = (struct udphdr*)(dgram + iphlen);
	header_size = sizeof(struct iphdr) + sizeof(struct udphdr);
	data = dgram + header_size;

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(SERVER_IP);
	srv_addr.sin_port = htons(SERVER_PORT);

	strncpy(data, "Hello Dmitry!", MAX_MSG_SIZE);

	udpplen = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data);

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = udpplen;
	iph->id = htonl(54321);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;
	iph->saddr = inet_addr("192.168.2.1");
	iph->daddr = srv_addr.sin_addr.s_addr;

	udph->source = htons(CLIENT_PORT);
	udph->dest = htons(SERVER_PORT);
	udph->len = htons(sizeof(struct udphdr) + strlen(data));
	udph->check = 0;

	sd_cln = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sd_cln == -1) {
		perror("Client: socket(client)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd_cln, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if (rtn == -1) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	srv_hst = gethostbyaddr((char *)&srv_addr.sin_addr.s_addr, 4, AF_INET);
	printf("Client: udp echo server %s:%d (hs: %s)\n", inet_ntoa(srv_addr.sin_addr), ntohs(srv_addr.sin_port),
	((srv_hst != NULL) ? srv_hst->h_name : ""));

	bts = sendto(sd_cln, dgram, udpplen, 0, (struct sockaddr*)&srv_addr, socket_len);
	if (bts == -1) {
		perror("Client: sendto(server)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	printf("Client: send message to server: '%s' (%d bytes)\n", data, (int)bts);

	while(1) {
		memset(dgram, 0, MAX_DGRAM_SIZE);

		bts = recvfrom(sd_cln, dgram, MAX_MSG_SIZE, 0, NULL, NULL);
		if (bts == -1) {
			perror("Client: recvfrom(server)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		iph = (struct iphdr *)dgram;
		iphlen = iph->ihl*4;

		udph = (struct udphdr*)(dgram + iphlen);
		header_size = iphlen + sizeof udph;

		if (udph->dest == htons(CLIENT_PORT)) {
			print_udp_packet((u_char*)dgram, sizeof(dgram));
			printf("\n\nClient: receive message from server: '%s' (%d bytes)\n\n", (dgram + header_size), (int)bts);

			break;
		}
	}

	exit(EXIT_SUCCESS);
}
