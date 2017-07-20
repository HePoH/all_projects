#include "../include/core.h"

void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd) {
	*ld = (DIR_INFO*) malloc(sizeof(DIR_INFO));
	*rd = (DIR_INFO*) malloc(sizeof(DIR_INFO));

	if (chdir(getenv("HOME")) == -1) {
		perror("chdir");
		exit(EXIT_FAILURE);
	}

	get_dir_info(getenv("HOME"), rd);

	if (chdir(getenv("PWD")) == -1) {
		perror("chdir");
		exit(EXIT_FAILURE);
	}

	get_dir_info(getenv("PWD"), ld);

	*cd = *ld;
}

void free_fil(DIR_INFO** di) {
	FILE_INFO_LIST *p = NULL, *q = NULL;
        p = (*di)->head_file;

        while(p) {
		q = p;
                p = p->next;
		free(q);
        }

	(*di)->head_file = NULL;
	(*di)->current_file = NULL;
	(*di)->tail_file = NULL;
}

void get_dir_info(char* path, DIR_INFO** di) {
	struct dirent **fnl;
	int fc;

	fc = scandir(path, &fnl, 0, alphasort);

	if (fc < 0)
		perror("scandir");
	else {
		int i = 1;

		if (getcwd((*di)->dir_path, BUF_SIZE) == NULL)
			strncpy((*di)->dir_path, "-", BUF_SIZE - 1);

		(*di)->file_count = fc - 2;

		if (fc > 1) {
			FILE_INFO_LIST* head = NULL;

			head = malloc(sizeof(FILE_INFO_LIST));
			if (head == NULL) {
				perror("malloc");
				exit(EXIT_FAILURE);
			}

			strncpy(head->file_name, fnl[i]->d_name, BUF_SIZE - 1);

			if (stat(fnl[i]->d_name, &head->file_stat) != 0) {
				perror("stat");
				exit(EXIT_FAILURE);
			}

			head->next = NULL;
			head->prev = NULL;

			(*di)->head_file = head;
			(*di)->current_file = head;

			i++;
		}

		for (; i < fc; i++) {
			FILE_INFO_LIST* fil = NULL;

			fil = (FILE_INFO_LIST*) malloc(sizeof(FILE_INFO_LIST));
			if (fil == NULL) {
				perror("malloc");
				exit(EXIT_FAILURE);
                        }

			strncpy(fil->file_name, fnl[i]->d_name, BUF_SIZE - 1);

			if (stat(fnl[i]->d_name, &fil->file_stat) != 0) {
				perror("stat");
				exit(1);
			}

			fil->next = NULL;
			fil->prev = (*di)->current_file;

			(*di)->current_file->next = fil;
			(*di)->current_file = fil;

			free(fnl[i]);
		}

		(*di)->tail_file = (*di)->current_file;
		(*di)->current_file = (*di)->head_file;

		free(fnl);
	}
}
