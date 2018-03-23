#include "../include/server_core.h"

void* cln_hndl(void* args) {
    int fd = 0, flags = 0, rtn = 0;
    client_info_t *cln_info = NULL;
    struct sctp_sndrcvinfo sndrcv_info;
    struct sctp_event_subscribe events;
    ssize_t bts = 0;
    char filename[64], ip_addr[INET_ADDRSTRLEN], msg[MAX_MSG_SIZE+1];

    if (args == NULL) {
        printf("invalid thread args\n");
        pthread_exit((void*)EXIT_FAILURE);
    }

    cln_info = (client_info_t *)args;

    memset(&events, 0, sizeof(events));
    memset(msg, 0, MAX_MSG_SIZE+1);

    events.sctp_data_io_event = 1;

    rtn = setsockopt(cln_info->sock, SOL_SCTP, SCTP_EVENTS, &events, sizeof(events));
    if (rtn == -1) {
        perror("setsockopt() failed");

        close(cln_info->sock);
        free(cln_info);

        pthread_exit((void*)EXIT_FAILURE);
    }

    if (inet_ntop(AF_INET, &(cln_info->addr.sin_addr),
                ip_addr, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop() failed");

        close(cln_info->sock);
        free(cln_info);

        pthread_exit((void*)EXIT_FAILURE);
    }

    rtn = snprintf(filename, 64, "./files/client_%s", ip_addr);
    if (rtn < 0) {
        perror("sprintf() failed");

        close(cln_info->sock);
        free(cln_info);

        pthread_exit((void*)EXIT_FAILURE);
    }

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open() failed");

        close(cln_info->sock);
        free(cln_info);

        pthread_exit((void*)EXIT_FAILURE);
    }

    while(1) {
        memset(msg, 0, MAX_MSG_SIZE);

        bts = sctp_recvmsg(cln_info->sock, msg, MAX_MSG_SIZE,
                (struct sockaddr *)NULL, 0, &sndrcv_info, &flags);
        if (bts == -1) {
            perror("sctp_recvmsg() failed");

            close(fd);
            close(cln_info->sock);

            free(cln_info);

            pthread_exit((void*)EXIT_FAILURE);
        }

        if (sndrcv_info.sinfo_stream == MSG_STREAM) {
            msg[MAX_MSG_SIZE] = 0;

            printf("recived message from client: '%s' (%d bytes)\n", msg, (int)bts);

            if (!strcmp(msg, "exit")) {
                printf("client message handler thread bye\n");

                close(fd);
                close(cln_info->sock);

                free(cln_info);

                pthread_exit((void*)EXIT_SUCCESS);
            }
        }
        else if (sndrcv_info.sinfo_stream == FILE_STREAM) {
            rtn = write(fd, &msg, bts);
            if(rtn != bts){
                perror("write() failed");

                close(fd);
                close(cln_info->sock);

                free(cln_info);

                pthread_exit((void*)EXIT_SUCCESS);

            }
        }
    }

    close(fd);
    close(cln_info->sock);

    free(cln_info);

    pthread_exit((void*)EXIT_SUCCESS);
}
