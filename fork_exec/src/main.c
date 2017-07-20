#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int main() {
	pid_t pid = 0;
	int status = 0;

	char *args[] = {"ls", "-l", NULL};

	/* Parent */
	printf("\nMain: pid: %d ppid: %d\n", getpid(), getppid());

	/* Child */
	pid = fork();
	if (pid == 0) {
		printf("Child: pid: %d ppid: %d\n\n", getpid(), getppid());

		execv("/bin/ls", args);
		exit(0);
	}


	wait(&status);
	printf("\n");

	return 0;
}
