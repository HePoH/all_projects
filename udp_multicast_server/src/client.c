#include "../include/client_core.h"

int main() {
	int sd_cln = 0, rtn = 0;
	struct sockaddr_in srv_addr;
	char msg[MAX_MSG_SIZE];
	socklen_t socket_len = 0;

	struct ip_mreq mreqs;

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(msg, 0, MAX_MSG_SIZE);

	sd_cln = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd_cln == -1) {
		perror("Client: socket(client)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	mreqs.imr_multiaddr.s_addr = inet_addr(MULTICAST_SERVER_IP);
	mreqs.imr_interface.s_addr = htonl(INADDR_ANY);

	setsockopt(sd_cln, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqs, sizeof(mreqs));

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(SERVER_PORT);

	rtn = bind(sd_cln, (struct sockaddr*)&srv_addr, socket_len);
	if (rtn == -1) {
		perror("Client: bind(server)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	while(1) {
		memset(msg, 0, MAX_MSG_SIZE);

		rtn = recvfrom(sd_cln, msg, MAX_MSG_SIZE, 0, NULL, NULL);
		if (rtn == -1) {
			perror("Client: recvfrom(server)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		printf("Client: receive message from server: '%s' (%d bytes)\n", msg, (int)strlen(msg));
	}

	exit(EXIT_SUCCESS);
}
