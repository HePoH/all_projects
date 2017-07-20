#include "../include/server_core.h"

int main() {
	int sd_tcp_srv = 0, sd_udp_srv = 0, sd_tcp_cln = 0, max_sd = 0, rtn = 0, on = 1;
	struct sockaddr_in srv_addr, cln_addr;
	struct hostent* cln_hst = NULL;
	socklen_t socket_len = 0;
	fd_set read_sds;

	pthread_t cln_tcp_hndl_tid = 0;
	CLIENT_INFO* cln_tcp_info = NULL;

	ssize_t bts = 0;
	char msg[MAX_MSG_SIZE];

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(&cln_addr, 0, socket_len);

	sd_tcp_srv = socket(AF_INET, SOCK_STREAM, 0);
	if (sd_tcp_srv == -1) {
		perror("[TCP] Server: socket(server)");

		close(sd_tcp_srv);
		exit(EXIT_FAILURE);
	}

	sd_udp_srv = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd_udp_srv == -1) {
		perror("[UDP] Server: socket(server)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	max_sd = (sd_udp_srv > sd_tcp_srv) ? sd_udp_srv : sd_tcp_srv;
	printf("TCP&UDP server start\n");

	rtn = setsockopt(sd_tcp_srv, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rtn == -1) {
		perror("[TCP] Server: setsockopt(SO_REUSEADDR)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd_udp_srv, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rtn == -1) {
		perror("[UDP] Server: setsockopt(SO_REUSEADDR)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(SERVER_IP);
	srv_addr.sin_port = htons(SERVER_PORT);

	rtn = bind(sd_tcp_srv, (struct sockaddr*)&srv_addr, socket_len);
	if (rtn == -1) {
		perror("[TCP] Server: bind(server)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	rtn = bind(sd_udp_srv, (struct sockaddr*)&srv_addr, socket_len);
	if (rtn == -1) {
		perror("[UDP] Server: bind(server)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	rtn = listen(sd_tcp_srv, 5);
	if (rtn == -1) {
		perror("[TCP] Server: listen(server)");

		close(sd_tcp_srv);
		close(sd_udp_srv);

		exit(EXIT_FAILURE);
	}

	while(1) {
		FD_ZERO(&read_sds);

		FD_SET(sd_tcp_srv, &read_sds);
		FD_SET(sd_udp_srv, &read_sds);

		rtn = select(max_sd + 1, &read_sds, NULL, NULL, NULL);
		if (rtn == -1) {
			perror("Server: select()");

			close(sd_tcp_srv);
			close(sd_udp_srv);

			exit(EXIT_FAILURE);
		}

		memset(&cln_addr, 0, socket_len);
		socket_len = sizeof(struct sockaddr_in);

		if (FD_ISSET(sd_tcp_srv, &read_sds)) {
			sd_tcp_cln = accept(sd_tcp_srv, (struct sockaddr*)&cln_addr, &socket_len);
			if (sd_tcp_srv == -1) {
				perror("[TCP] Server: accept(client)");

				close(sd_tcp_srv);
				close(sd_udp_srv);

				exit(EXIT_FAILURE);
			}

			cln_hst = gethostbyaddr((char *)&cln_addr.sin_addr.s_addr, 4, AF_INET);
			printf("[TCP] Server: incoming connection from %s:%d (hs: %s)\n\n", inet_ntoa(cln_addr.sin_addr), ntohs(cln_addr.sin_port),
			((cln_hst != NULL) ? cln_hst->h_name : ""));

			cln_tcp_info = malloc(sizeof(CLIENT_INFO));
			cln_tcp_info->sd_cln = sd_tcp_cln;
			memcpy(&cln_tcp_info->cln_addr, &cln_addr, socket_len);

			pthread_create(&cln_tcp_hndl_tid, NULL, cln_tcp_hndl, (void*)cln_tcp_info);
		}
		else
			if (FD_ISSET(sd_udp_srv, &read_sds)) {
				memset(msg, 0, MAX_MSG_SIZE * sizeof(char));

				bts = recvfrom(sd_udp_srv, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&cln_addr, &socket_len);
				if (bts == -1) {
					perror("[UDP] Server: recvfrom(client)");

					close(sd_tcp_srv);
					close(sd_udp_srv);

					exit(EXIT_FAILURE);
				}

				cln_hst = gethostbyaddr((char *)&cln_addr.sin_addr.s_addr, 4, AF_INET);
				printf("[UDP] Server: incoming connection from %s:%d (hs: %s)\n\n", inet_ntoa(cln_addr.sin_addr), ntohs(cln_addr.sin_port),
				((cln_hst != NULL) ? cln_hst->h_name : ""));

				printf("[UDP] Server: udp recived message from client: '%s' (%d bytes)\n", msg, (int)bts);

				if (!strcmp(msg, "exit")) {
					printf("Server: main thread bye\n");

					close(sd_tcp_srv);
					close(sd_udp_srv);

					exit(EXIT_FAILURE);
				}

				bts = sendto(sd_udp_srv, msg, bts, 0, (struct sockaddr*)&cln_addr, socket_len);
				if (bts == -1) {
					perror("[UDP] Server: sendto(client)");

					close(sd_tcp_srv);
					close(sd_udp_srv);

					exit(EXIT_FAILURE);
				}

				printf("[UDP] Server: send message to client: '%s' (%d bytes)\n\n", msg, (int)bts);
			}
	}

	close(sd_tcp_srv);
	close(sd_udp_srv);

	exit(EXIT_SUCCESS);
}
