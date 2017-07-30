#include "../include/client_core.h"

int main() {
	int sd_cln = 0, header_size = 0;
	unsigned short iphdrlen = 0, udpplen = 0;
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

	udph = (struct udphdr*)dgram;
	data = dgram + sizeof(struct udphdr);

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(SERVER_IP);
	srv_addr.sin_port = htons(SERVER_PORT);

	strncpy(data, "Hello Dmitry!", MAX_MSG_SIZE);

	udph->source = htons(CLIENT_PORT);
	udph->dest = htons(SERVER_PORT);
	udph->len = htons(sizeof(struct udphdr) + strlen(data));
	udph->check = 0;

	udpplen = sizeof(struct udphdr) + strlen(data);

	sd_cln = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sd_cln == -1) {
		perror("Client: socket(client)");

		close(sd_cln);
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
		iphdrlen = iph->ihl*4;

		udph = (struct udphdr*)(dgram + iphdrlen);
		header_size = iphdrlen + sizeof(struct udphdr);

		if (udph->dest == htons(CLIENT_PORT)) {
			print_udp_packet((u_char*)dgram, sizeof(dgram));
			printf("\n\nClient: receive message from server: '%s' (%d bytes)\n\n", (dgram + header_size), (int)bts);

			break;
		}
	}

	exit(EXIT_SUCCESS);
}
