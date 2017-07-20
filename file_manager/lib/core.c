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

char* trim(char* spaced) {
    int length = strlen(spaced);

    while (isspace(spaced[length - 1]))
        --length;
    while (*spaced && isspace(*spaced))
        ++spaced, --length;

    return strndup(spaced, length);
}

void* copy_file(void* args) {
	int fd_src = 0, fd_dst = 0;
	char buf[COPY_BUF_SIZE];
	ssize_t nbytes = 0;
	COPY_FILE_INFO* cfi = NULL;

	cfi = (COPY_FILE_INFO*) args;

	fd_src = open(cfi->fn_src, O_RDONLY);
	if (fd_src == -1) {
		perror("open");
		pthread_exit(EXIT_FAILURE);
	}

	if (stat(cfi->fn_src, &cfi->fs_src) != 0) {
		perror("stat");
		pthread_exit(EXIT_FAILURE);
	}

	fd_dst = open(cfi->fn_dst, O_CREAT | O_WRONLY | O_TRUNC, cfi->fs_src.st_mode);
	if (fd_dst == -1) {
		perror("open");
		pthread_exit(EXIT_FAILURE);
	}

	cfi->cur_size = 0;
	while((nbytes = read(fd_src, buf, COPY_BUF_SIZE)) > 0) {
		if (write(fd_dst, buf, nbytes) != nbytes) {
			perror("write");
			pthread_exit(EXIT_FAILURE);
		}

		cfi->cur_size += nbytes;
	}

	if (nbytes == -1) {
		perror("write");
		pthread_exit(EXIT_FAILURE);
	}

	close(fd_src);
	close(fd_dst);

	pthread_exit(EXIT_SUCCESS);
}

void* display_copy_status(void* args) {
	int st_bl = 0, st_bl_c = 0, stat = 0, pr = 0;
	COPY_FILE_INFO* cfi = NULL;

	cfi = (COPY_FILE_INFO*) args;
	st_bl = cfi->cols-12;
	st_bl_c = cfi->fs_src.st_size / st_bl;

	wclear(cfi->s_wnd);
	box(cfi->s_wnd, 0, 0);

	mvwprintw(cfi->s_wnd, 1, 2, "[");
	mvwprintw(cfi->s_wnd, 1, cfi->cols-9, "]");
	wrefresh(cfi->s_wnd);

	while(cfi->cur_size < cfi->fs_src.st_size ) {
		stat = cfi->cur_size / st_bl_c + 3;
		pr = ((double)cfi->cur_size/(double)cfi->fs_src.st_size)*100;
		mvwprintw(cfi->s_wnd, 1, stat, "#");
		mvwprintw(cfi->s_wnd, 1, cfi->cols-7, "%d%%", pr);
		wrefresh(cfi->s_wnd);
	}

	mvwprintw(cfi->s_wnd, 1, cfi->cols-7, "100%%");
	wrefresh(cfi->s_wnd);

	pthread_exit(EXIT_SUCCESS);
}

char* get_file_path(char* file_name) {
	char* file_path = NULL, cur_dir[BUF_SIZE];

        file_path = malloc(BUF_SIZE * sizeof(char));

        getcwd(cur_dir, BUF_SIZE);
        snprintf(file_path, BUF_SIZE, "%s/%s", cur_dir, file_name);

        return file_path;
}
