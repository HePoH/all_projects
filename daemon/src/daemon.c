#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 256

#define LOG_FILE_NAME "/home/2017/d_medvedev/work/daemon/log/time_daemon.log"
#define PID_FILE_NAME "/home/2017/d_medvedev/work/daemon/fifo/time_daemon.pid"
#define DT_FILE_NAME  "/home/2017/d_medvedev/work/daemon/fifo/dt.pipe"

void send_pid(char* fn, pid_t pid) {
	int fd = 0;

	if (mkfifo(fn, 0666) == -1) {
		perror("mkfifo");
		/*exit(EXIT_FAILURE);*/
	}

	fd = open(fn, O_WRONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	write(fd, &pid, sizeof(pid_t));
	close(fd);
}

char* get_cur_dt(char* format) {
	time_t rawtime;
	struct tm* timeinfo = NULL;
	char* date_time = NULL;

	date_time = malloc(BUF_SIZE * sizeof(char));
	if (date_time == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(date_time, BUF_SIZE, format, timeinfo);

	return date_time;
}

void sys_log(char* msg) {
	char *cur_dt = NULL, sys_msg[BUF_SIZE];

	cur_dt = get_cur_dt("[%D] [%T]");
	snprintf(sys_msg, BUF_SIZE, "%s: %s\n", cur_dt, msg);

	write(STDOUT_FILENO, sys_msg, strlen(sys_msg));
}

int main(int argc, char** argv) {
	pid_t dmn_pid = 0;
	sigset_t sig_set;
	int fd_log = 0, fd_dt = 0, sig_num = 0;
	char* cur_dt = NULL, msg[BUF_SIZE];

	dmn_pid = fork();
	if (dmn_pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (!dmn_pid) {
		if (setsid() == -1) {
			perror("chdir");
			exit(EXIT_FAILURE);
		}

		if (chdir("/") == -1) {
			perror("chdir");
			exit(EXIT_FAILURE);
		}

		umask(0);

		close(STDIN_FILENO);

		fd_log = open(LOG_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);
		if (fd_log) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		dup2(fd_log, STDOUT_FILENO);
		dup2(fd_log, STDERR_FILENO);
		close(fd_log);

		send_pid(PID_FILE_NAME, getpid());

		if (mkfifo(DT_FILE_NAME, 0666) == -1) {
			perror("mkfifo");
			/*exit(EXIT_FAILURE);*/
		}

		fd_dt = open(DT_FILE_NAME, O_WRONLY);
		if (fd_dt == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		sigemptyset(&sig_set);
		sigaddset(&sig_set, SIGUSR1);
		sigaddset(&sig_set, SIGHUP);
		sigprocmask(SIG_BLOCK, &sig_set, NULL);

		while (1) {
			sigwait(&sig_set, &sig_num);

			snprintf(msg, BUF_SIZE, "%s: %d", "Received signal", sig_num);
			sys_log(msg);

			switch(sig_num) {
				case 1:
					break;

				case 10:
					sys_log("Received a request");

					cur_dt = get_cur_dt("%D %T");
					write(fd_dt, cur_dt, 18);
					free(cur_dt);

					snprintf(msg, BUF_SIZE, "%s: %s", "Send reply", cur_dt);
					sys_log(msg);

					break;

				default:
					break;
			}

			fflush(NULL);
			sync();
		}

		close(fd_log);
		close(fd_dt);
	}

	exit(EXIT_SUCCESS);
}
