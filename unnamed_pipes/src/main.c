#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int main() {
	int fd_ps[2];
	pid_t ls_pid = 0, cut_pid = 0;
	int status = 0;

	pipe(fd_ps);

	/* main */
	printf("\n(main): pid: %d ppid: %d\n", getpid(), getppid());

	/* ls */
	ls_pid = fork();
	if (ls_pid == 0) {
		printf("(ls): pid: %d ppid: %d\n", getpid(), getppid());

		close(fd_ps[0]);
		dup2(fd_ps[1], STDOUT_FILENO);

		execl("/bin/ls", "ls", "-l", NULL);
		perror("execl");

		exit(EXIT_FAILURE);
	}

	waitpid(ls_pid);

	/* cut */
	cut_pid = fork();
	if (cut_pid == 0) {
		printf("(cut): pid: %d ppid: %d\nCut:\n", getpid(), getppid());

		close(fd_ps[1]);
		dup2(fd_ps[0], STDIN_FILENO);

		execl("/usr/bin/cut", "cut", "-d ", "-f10", NULL);
		perror("execl");

		exit(EXIT_FAILURE);

	}

	/*wait(&status);
	wait(&status);*/

	printf("\n");

	exit(EXIT_SUCCESS);
}
