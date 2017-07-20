#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int main() {
	pid_t pid = 0;
	int status = 0;

	/* 1 */
	printf("\n(1): pid: %d ppid: %d\n", getpid(), getppid());

	/* 2 */
	pid = fork();
	if (pid == 0) {
		printf("(2): pid: %d ppid: %d\n", getpid(), getppid());

		/* 4 */
		pid = fork();
		if (pid == 0) {
			printf("(4): pid: %d ppid: %d\n", getpid(), getppid());
			exit(0);
		}

		wait(&status);
		exit(0);
	}

	/* 3 */
	pid = fork();
	if (pid == 0) {
		printf("(3): pid: %d ppid: %d\n", getpid(), getppid());

		/* 5 */
		pid = fork();
		if (pid == 0) {
			printf("(5): pid: %d ppid: %d\n", getpid(), getppid());
			exit(0);
		}

		/* 6 */
		pid = fork();
		if (pid == 0) {
			printf("(6): pid: %d ppid: %d\n", getpid(), getppid());
			exit(0);
		}

		wait(&status);
		wait(&status);
		exit(0);
	}

	wait(&status);
	wait(&status);

	printf("\n");

	return 0;
}
