#include "../include/client_core.h"

int main(int argc, char *argv[]) {
    int sock = 0, opt = 0, rtn = 0;
    struct sockaddr_in srv_addr;
    server_info_t srv_info;
    pthread_t file_hndl_tid = 0, msg_hndl_tid = 0;
    char ipv4_addr[INET_ADDRSTRLEN];
    unsigned short port;
    socklen_t socket_len = 0;

    if (argc < 4) {
        printf("usage: %s -d [dest address] -p [port num] -f [filename]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    socket_len = sizeof(struct sockaddr_in);

    memset(&srv_addr, 0, socket_len);
    memset(&srv_info, 0, sizeof(srv_info));

    srv_addr.sin_family = AF_INET;

    while((opt = getopt (argc, argv, "d:p:f:")) != -1) {
        switch (opt) {
            case 'd':
                rtn = sscanf(optarg, "%[^:]", ipv4_addr);
                if (rtn != 1) {
                    printf("sscanf() failed\n");
                    return -1;
                }

                rtn = inet_pton(AF_INET, (char*)ipv4_addr,
                        (void*)&(srv_addr.sin_addr));
                if (rtn == -1) {
                    perror("inet_pton() failed");
                    return -1;
                }
                break;
            case 'p':
                rtn = sscanf(optarg, "%hu", &port);
                if (rtn != 1) {
                    printf("sscanf() failed\n");
                    return -1;
                }

                srv_addr.sin_port = htons(port);
                break;

            case 'f':
                strcpy(srv_info.filename, optarg);
                break;
        }
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sock == -1) {
        perror("socket() failed");

        close(sock);
        exit(EXIT_FAILURE);
    }

    rtn = connect(sock, (struct sockaddr *)&srv_addr, socket_len);
    if (rtn == -1) {
        perror("connect() failed");

        close(sock);
        exit(EXIT_FAILURE);
    }

    srv_info.sock = sock;
    srv_info.addr = srv_addr;

    pthread_create(&file_hndl_tid, NULL, file_hndl, (void*)&srv_info);
    pthread_create(&msg_hndl_tid, NULL, msg_hndl, (void*)&srv_info);

    pthread_join(file_hndl_tid, NULL);
    pthread_join(msg_hndl_tid, NULL);

    close(sock);

    exit(EXIT_SUCCESS);
}
