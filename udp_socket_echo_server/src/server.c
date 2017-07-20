#include "../include/server_core.h"

int main() {
	int sd_srv = 0, rtn = 0;
	struct sockaddr_in srv_addr, cln_addr;
	struct hostent* cln_hst = NULL;
	socklen_t socket_len = 0;
	ssize_t bts = 0;
	char msg[MAX_MSG_SIZE];

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(&cln_addr, 0, socket_len);

	sd_srv = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd_srv == -1) {
		perror("Server: socket(server)");

		close(sd_srv);
		exit(EXIT_FAILURE);
	}

	printf("Server start\n");

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(SERVER_PORT);

	rtn = bind(sd_srv, (struct sockaddr*)&srv_addr, socket_len);
	if (rtn == -1) {
		perror("Server: bind(server)");

		close(sd_srv);
		exit(EXIT_FAILURE);
	}

	while(1) {
		socket_len = sizeof(struct sockaddr_in);
		memset(msg, 0, MAX_MSG_SIZE * sizeof(char));

		bts = recvfrom(sd_srv, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&cln_addr, &socket_len);
		if (bts == -1) {
			perror("Server: recvfrom(client)");

			close(sd_srv);
			exit(EXIT_FAILURE);
		}

		cln_hst = gethostbyaddr((char *)&cln_addr.sin_addr.s_addr, 4, AF_INET);
		printf("Server: incoming connection from %s:%d (hs: %s)\n", inet_ntoa(cln_addr.sin_addr), ntohs(cln_addr.sin_port),
		((cln_hst != NULL) ? cln_hst->h_name : ""));

		printf("Server: recived message from client: '%s' (%d bytes)\n", msg, (int) bts);

		if (!strcmp(msg, "exit")) {
			printf("Server: main thread bye\n");

			close(sd_srv);
			exit(EXIT_FAILURE);
		}

		bts = sendto(sd_srv, msg, bts, 0, (struct sockaddr*)&cln_addr, socket_len);
		if (bts == -1) {
			perror("Server: sendto(client)");

			close(sd_srv);
			exit(EXIT_FAILURE);
		}

		printf("Server: send message to client: '%s' (%d bytes)\n\n", msg, (int)bts);
	}

	close(sd_srv);
	exit(EXIT_SUCCESS);
}
