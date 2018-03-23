#include "../include/server_core.h"

int main() {
    int srv_sock = 0, cln_sock = 0, rtn = 0;
    struct sockaddr_in srv_addr, cln_addr;
    socklen_t socket_len = 0;
    pthread_t cln_hndl_tid = 0;
    client_info_t *cln_info = NULL;

    socket_len = sizeof(struct sockaddr_in);

    memset(&srv_addr, 0, socket_len);
    memset(&cln_addr, 0, socket_len);

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(DEF_SERVER_PORT);

    srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (srv_sock == -1) {
        perror("socket() failed");

        close(srv_sock);
        exit(EXIT_FAILURE);
    }

    rtn = bind(srv_sock, (struct sockaddr*)&srv_addr, socket_len);
    if (rtn == -1) {
        perror("bind() failed");

        close(srv_sock);
        exit(EXIT_FAILURE);
    }

    rtn = listen(srv_sock, 1);
    if (rtn == -1) {
        perror("listen() failed");

        close(srv_sock);
        exit(EXIT_FAILURE);
    }

    printf("server start...\n");

    while(1) {
        socket_len = sizeof(struct sockaddr_in);

        cln_sock = accept(srv_sock, (struct sockaddr*)&cln_addr, &socket_len);
        if (cln_sock == -1) {
            perror("accept() failed");

            close(srv_sock);
            close(cln_sock);

            exit(EXIT_FAILURE);
        }

        cln_info = malloc(sizeof(client_info_t));
        cln_info->sock = cln_sock;
        cln_info->addr = cln_addr;

        pthread_create(&cln_hndl_tid, NULL, cln_hndl, (void*)cln_info);
    }

    close(srv_sock);
    exit(EXIT_SUCCESS);
}
