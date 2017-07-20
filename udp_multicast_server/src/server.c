#include "../include/server_core.h"

int main() {
	int sd_srv = 0, rtn = 0, on = 1;
	struct sockaddr_in srv_addr;
	socklen_t socket_len = 0;
	ssize_t bts = 0;
	char msg[MAX_MSG_SIZE];

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(msg, 0, MAX_MSG_SIZE * sizeof(char));

	strncpy(msg, "multicast message", strlen("multicast message"));

	sd_srv = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd_srv == -1) {
		perror("Server: socket(server)");

		close(sd_srv);
		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd_srv, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rtn == -1) {
		perror("Server: setsockopt(SO_REUSEADDR)");

		close(sd_srv);
		exit(EXIT_FAILURE);
	}

	printf("Server start\n");

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	srv_addr.sin_port = htons(SERVER_PORT);

	while(1) {
		bts = sendto(sd_srv, msg, strlen(msg), 0, (struct sockaddr*)&srv_addr, socket_len);
		if (bts == -1) {
			perror("Server: sendto(client)");

			close(sd_srv);
			exit(EXIT_FAILURE);
		}

		printf("Server: send broadcast message: '%s' (%d bytes)\n", msg, (int)bts);

		sleep(5);
	}

	close(sd_srv);
	exit(EXIT_SUCCESS);
}
