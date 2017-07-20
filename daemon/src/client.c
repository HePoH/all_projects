#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 256

#define LOG_FILE_NAME "/home/2017/d_medvedev/work/daemon/log/time_daemon.log"
#define PID_FILE_NAME "/home/2017/d_medvedev/work/daemon/fifo/time_daemon.pid"
#define DT_FILE_NAME "/home/2017/d_medvedev/work/daemon/fifo/dt.pipe"

int main() {
	pid_t dmn_pid = 0;
	int ch = 0, fd_dmn_pid = 0, fd_dt = 0;
	char cur_dt[BUF_SIZE];

	fd_dmn_pid = open(PID_FILE_NAME, O_RDONLY);
	if (fd_dmn_pid == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	read(fd_dmn_pid, &dmn_pid, sizeof(pid_t));
	printf("Daemon pid: %d\n", dmn_pid);

	fd_dt = open(DT_FILE_NAME, O_RDONLY);
	if (fd_dt == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	while((ch = getchar()) != 32) {
		switch(ch) {
			case 10:
				kill(dmn_pid, 10);
				printf("Send signal SIGUSER1(10)");

				read(fd_dt, &cur_dt, 18);
				printf("\nCurrent date time: %s\n", cur_dt);

				break;
		}
	}

	close(fd_dmn_pid);
	unlink(PID_FILE_NAME);

	exit(EXIT_SUCCESS);
}
