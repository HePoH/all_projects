#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 256

int main() {
	pid_t srv_pid = 0;
	int ch = 0, fd_fpid = 0;
	char* pid_fn = "fs_pid";

	fd_fpid = open(pid_fn, O_RDONLY);
	if (fd_fpid == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	read(fd_fpid, &srv_pid, sizeof(pid_t));
	printf("Server pid: %d\n", srv_pid);

	while((ch = getchar()) != 32) {
		switch(ch) {
			case 10:
				kill(srv_pid, 10);
				printf("Send signal SIGUSER1(10)");
				break;
		}
	}

	close(fd_fpid);
	unlink(pid_fn);

	exit(EXIT_SUCCESS);
}
