#ifndef CORE_H
#define CORE_H

#define _DEFAULT_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE

#define BUF_SIZE 255
#define COPY_BUF_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <fcntl.h>

#include <pthread.h>

#include <curses.h>

typedef struct fil {
	char file_name[BUF_SIZE];
	struct stat file_stat;

	struct fil *next;
	struct fil *prev;
} FILE_INFO_LIST;

typedef struct di {
	char dir_path[BUF_SIZE];
	int file_count;

	FILE_INFO_LIST* head_file;
	FILE_INFO_LIST* current_file;
	FILE_INFO_LIST* tail_file;

	WINDOW *p_wnd;
	WINDOW *fn_wnd, *ow_wnd, *s_wnd, *mt_wnd, *pr_wnd;
} DIR_INFO;

typedef struct cfi {
	char fn_src[BUF_SIZE];
	char fn_dst[BUF_SIZE];

	long cur_size;
	struct stat fs_src;

	WINDOW *s_wnd;
	int cols, rows;
} COPY_FILE_INFO;

void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd);
void free_fil(DIR_INFO** di);
void get_dir_info(char* path, DIR_INFO** di);
char* get_file_path(char* file_name);
void* copy_file(void* args);
void* display_copy_status(void* args);
char* trim(char* spaced);

#endif
