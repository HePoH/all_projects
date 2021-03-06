#include "../include/client_core.h"

int main() {
	int sd_cln = 0, rtn = 0;
	struct sockaddr_in srv_addr;
	struct hostent* srv_hst = NULL;
	char msg[MAX_MSG_SIZE];
	socklen_t socket_len = 0;
	ssize_t bts = 0;

	socket_len = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, socket_len);

	sd_cln = socket(AF_INET, SOCK_STREAM, 0);
	if (sd_cln == -1) {
		perror("Client: socket(client)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	srv_addr.sin_port = htons(SERVER_PORT);

	rtn = connect(sd_cln, (struct sockaddr *)&srv_addr, socket_len);
	if (rtn == -1) {
		perror("Client: connect(server)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	srv_hst = gethostbyaddr((char *)&srv_addr.sin_addr.s_addr, 4, AF_INET);
	printf("Client: connected to the tcp echo server %s:%d (hs: %s)\n", inet_ntoa(srv_addr.sin_addr), ntohs(srv_addr.sin_port),
	((srv_hst != NULL) ? srv_hst->h_name : ""));

	while(1) {
		memset(msg, 0, MAX_MSG_SIZE);

		printf("Input you message: ");
		if (fgets(msg, MAX_MSG_SIZE, stdin) == NULL) {
			perror("Client: fgets(msg)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		msg[strlen(msg) - 1] = '\0';

		bts = send(sd_cln, msg, strlen(msg), 0);
		if (bts == -1) {
			perror("Client: send(server)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		printf("Client: send message to server: '%s' (%d bytes)\n", msg, (int)bts);

		if(!strcmp(msg, "exit")){
			printf("Client: main thread bye\n");
			exit(EXIT_SUCCESS);
		}

		memset(msg, 0, MAX_MSG_SIZE);

		bts = recv(sd_cln, msg, MAX_MSG_SIZE, 0);
		if (bts == -1) {
			perror("Client: recv(server)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		printf("Client: receive message from server: '%s' (%d bytes)\n\n", msg, (int)bts);
	}

	exit(EXIT_SUCCESS);
}
