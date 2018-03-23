#include "../include/client_core.h"

void* file_hndl(void* args) {
    int fd = 0, rtn = 0;
    server_info_t *srv_info = NULL;
    ssize_t bts = 0;
    char msg[MAX_MSG_SIZE+1];

    if (args == NULL) {
        printf("invalid thread args\n");
        pthread_exit((void*)EXIT_FAILURE);
    }

    srv_info = (server_info_t *)args;
    memset(msg, 0, MAX_MSG_SIZE+1);

    fd = open(srv_info->filename, O_RDONLY);
    if (fd == -1) {
        perror("open() failed");
        pthread_exit((void*)EXIT_FAILURE);
    }

    while(1) {
        memset(msg, 0, MAX_MSG_SIZE);

        rtn = read(fd, msg, MAX_MSG_SIZE);
        if (rtn == -1) {
            perror("read() failed");

            close(fd);
            pthread_exit((void*)EXIT_FAILURE);

        }

        if (rtn == 0)
            break;

        bts = sctp_sendmsg(srv_info->sock, msg, rtn, NULL, 0, 0, 0,
                FILE_STREAM, 0, 0);
        if (bts == -1) {
            perror("send() failed");

            close(fd);
            pthread_exit((void*)EXIT_FAILURE);
        }
    }

    close(fd);
    pthread_exit((void*)EXIT_SUCCESS);
}

void* msg_hndl(void* args) {
    server_info_t *srv_info = NULL;
    ssize_t bts = 0;
    char msg[MAX_MSG_SIZE+1];

    if (args == NULL) {
        printf("invalid thread args\n");
        pthread_exit((void*)EXIT_FAILURE);
    }

    srv_info = (server_info_t *)args;
    memset(msg, 0, MAX_MSG_SIZE+1);

    while(1) {
        memset(msg, 0, MAX_MSG_SIZE);

        printf("message: ");
        if (fgets(msg, MAX_MSG_SIZE, stdin) == NULL) {
            perror("fgets() failed");
            pthread_exit((void*)EXIT_FAILURE);
        }

        msg[strlen(msg)-1] = 0;

        bts = sctp_sendmsg(srv_info->sock, msg, strlen(msg), NULL, 0, 0, 0,
                MSG_STREAM, 0, 0);
        if (bts == -1) {
            perror("send() failed");
            pthread_exit((void*)EXIT_FAILURE);
        }

        printf("send message to server: '%s' (%d bytes)\n", msg, (int)bts);

        if(!strcmp(msg, "exit")){
            printf("message thread bye\n");
            pthread_exit((void*)EXIT_FAILURE);
        }
    }

    pthread_exit((void*)EXIT_SUCCESS);
}
