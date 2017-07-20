#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <signal.h>
#include <fcntl.h>
#include <errno.h>

int main() {
	int fd_fpid = 0, sig = 0;
	char* pid_fn = "fs_pid";

	pid_t srv_pid = 0;
	sigset_t sig_set;

	if (mkfifo(pid_fn, 0666) == -1) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	fd_fpid = open(pid_fn, O_WRONLY);
	if (fd_fpid == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	srv_pid = getpid();
	write(fd_fpid, &srv_pid, sizeof(pid_t));

	printf("Server pid: %d\n", srv_pid);

	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sig_set, NULL);

	while(1) {
		sigwait(&sig_set, &sig);
		printf("Receive signal!\n");
	}

	/*close(fd_fpid);
	unlink(fd_fpid);*/

	exit(EXIT_SUCCESS);
}
