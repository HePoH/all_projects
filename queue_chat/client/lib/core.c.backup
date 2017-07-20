#include "../include/core.h"

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void make_key() {
	int rtn;

	rtn = pthread_key_create(&key, destructor);
	if (rtn != 0) {
		perror("pthread_key_create");
		pthread_exit((void*)EXIT_FAILURE);
	}
}

void destructor(void* ptr) {
	free(ptr);
}

char* get_cur_dt(char* format) {
	time_t rt;
	struct tm* ti;
	char* dt;
	int rtn;

	rtn = pthread_once(&key_once, make_key);
	if (rtn != 0) {
		perror("pthread_once");
		pthread_exit((void*)EXIT_FAILURE);
	}

	dt = pthread_getspecific(key);
	if (dt == NULL) {
		dt = malloc(MAX_TIME_SIZE * sizeof(char));

		rtn = pthread_setspecific(key, dt);
		if (rtn != 0) {
			perror("pthread_setspecific");
			pthread_exit((void*)EXIT_FAILURE);
		}
	}

	time(&rt);
	ti = localtime(&rt);

	strftime(dt, MAX_TIME_SIZE, format, ti);

	return dt;
}

void sys_log(char* msg, int type, int fd) {
	char *cur_dt, sys_msg[MAX_LOG_MSG_SIZE];
	cur_dt = get_cur_dt("[%D][%T]");

	switch(type) {
		case INFO:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "INFORMATION", msg);
			break;

		case WARNING:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "WARNING", msg);
			break;

		case ERROR:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "ERROR", msg);
			break;
	}

	write(fd, &sys_msg, strlen(sys_msg));
}


void* srv_event_hndl(void* args) {
	mqd_t qd_srv;
        pid_t cln_pid;
	ssize_t bts_num;
        SERVER_MSG srv_msg;
	char* cln_name;

	cln_name = (char*) args;
	cln_pid = getpid();

	qd_srv = mq_open(SERVER_QUEUE_NAME, O_RDWR);
	if (qd_srv == -1) {
		perror("Client: mq_open(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	srv_msg.msg_type = CONNECTED;
	strncpy(srv_msg.data_text[0], cln_name, MAX_NAME_SIZE);
	srv_msg.data_pid = cln_pid;

	int srv_pid = 1;

	bts_num = mq_send(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, srv_pid);
	if (bts_num == -1) {
		perror("Client: mq_send(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	sys_log("Client: mq_send(server) send message to server", INFO, STDOUT_FILENO);
        printf("Struct: SERVER_MSG { msg_type = %d, data = %s, pid = %d }\n", srv_msg.msg_type, srv_msg.data_text[0], srv_msg.data_pid);

	while (1) {
		bts_num = mq_receive(qd_srv, (char*)&srv_msg, MAX_MSG_SIZE, &cln_pid);
		if (bts_num == -1) {
			perror("Client: mq_receive(server)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		printf("SJDNSDNSAJKDNASDNASK: %d\n\n", cln_pid);

		sys_log("Client: mq_receive(server) message received", INFO, STDOUT_FILENO);
		printf("Struct: SERVER_MSG { msg_type = %d, data[0] = %s, data[1] = %s, pid = %d }\n", srv_msg.msg_type, srv_msg.data_text[0], srv_msg.data_text[1], srv_msg.data_pid);

		/*bts_num = mq_send(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, SERVER_PID);
		if (bts_num == -1) {
			perror("Client: mq_send(server)");
			continue;
		}

		sys_log("Client: mq_send(server) send message to server", INFO, STDOUT_FILENO);
	        printf("Struct: SERVER_MSG { msg_type = %d, data[0] = %s, data[1] = %s, pid = %d }\n", srv_msg.msg_type, srv_msg.data_text[0], srv_msg.data_text[1],  srv_msg.data_pid);*/
	}

	if (mq_close(qd_srv) == -1) {
		perror("Client: mq_close(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	sys_log("Client: server thread bye", INFO, STDOUT_FILENO);
        pthread_exit((void*)EXIT_SUCCESS);
}

void* cln_inc_msg_hndl(void* args) {
	mqd_t qd_cln;
	ssize_t bts_num;
	CLIENT_MSG cln_msg;
	char* cln_name;

	cln_name = (char*) args;

	qd_cln = mq_open(CLIENT_QUEUE_NAME, O_RDWR);
	if (qd_cln == -1) {
		perror("Client: mq_open(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	/*cln_msg.msg_type = 1;
	cln_msg.snd_pid = 1;
	cln_msg.rcp_pid = 1;

	bts_num = mq_send(qd_cln, (char*)&cln_msg, CLIENT_MSG_SIZE, 0);
	if (bts_num == -1) {
		perror("Client: mq_send(client)");
	}*/

	while (1) {
		bts_num = mq_receive(qd_cln, (char*)&cln_msg, MAX_MSG_SIZE, NULL);
		if (bts_num == -1) {
			perror("Client: mq_receive(client)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		sys_log("Client: mq_receive(client) message received", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);

		/*bts_num = mq_send(qd_cln, (char*)&cln_msg, CLIENT_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Client: mq_send(client)");
			continue;
		}

		sys_log("Client: mq_send(client) response sent to client", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);*/
	}

	if (mq_close(qd_cln) == -1) {
		perror("Client: mq_close(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	sys_log("Client: client thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}

void* cln_out_msg_hndl(void* args) {
	mqd_t qd_cln;
	CLIENT_MSG cln_msg;
	char* cln_name, str_msg[MAX_MSG_SIZE];

	cln_name = (char*) args;

	qd_cln = mq_open(CLIENT_QUEUE_NAME, O_RDWR);
	if (qd_cln == -1) {
		perror("Client: mq_open(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	while(1) {
		fgets(str_msg, MAX_MSG_SIZE, stdin);

		if (str_msg[strlen(str_msg) - 1] == '\n')
			str_msg[strlen(str_msg) - 1] = '\0';

                if (strcmp("quit", str_msg) == 0 || strcmp("exit", str_msg) == 0)
                        pthread_exit((void*)EXIT_SUCCESS);
	}

	if (mq_close(qd_cln) == -1) {
		perror("Client: mq_close(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	sys_log("Client: client thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}
