#include "../include/server_core.h"

int main() {
	int sd_srv = 0, sd_cln = 0, rtn = 0;
	struct sockaddr_in srv_addr, cln_addr;
	struct hostent* cln_hst = NULL;
	socklen_t socket_len = 0;
	pthread_t cln_hndl_tid = 0;
	CLIENT_INFO* cln_info = NULL;

	socket_len = sizeof(struct sockaddr_in);

	memset(&srv_addr, 0, socket_len);
	memset(&cln_addr, 0, socket_len);

	sd_srv = socket(AF_INET, SOCK_STREAM, 0);
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

	rtn = listen(sd_srv, 1);
	if (rtn == -1) {
		perror("Server: listen(server)");

		close(sd_srv);
		exit(EXIT_FAILURE);
	}

	while(1) {
		socket_len = sizeof(struct sockaddr_in);

		sd_cln = accept(sd_srv, (struct sockaddr*)&cln_addr, &socket_len);
		if (sd_cln == -1) {
			perror("Server: accept(client)");

			close(sd_srv);
			close(sd_cln);

			exit(EXIT_FAILURE);
		}

		cln_hst = gethostbyaddr((char *)&cln_addr.sin_addr.s_addr, 4, AF_INET);
		printf("Server: incoming connection from %s:%d (hs: %s)\n\n", inet_ntoa(cln_addr.sin_addr), ntohs(cln_addr.sin_port),
		((cln_hst != NULL) ? cln_hst->h_name : ""));

		cln_info = malloc(sizeof(CLIENT_INFO));
		cln_info->sd_cln = sd_cln;
		memcpy(&cln_info->cln_addr, &cln_addr, socket_len);

		pthread_create(&cln_hndl_tid, NULL, cln_hndl, (void*)cln_info);
	}

	close(sd_srv);
	exit(EXIT_SUCCESS);
}
