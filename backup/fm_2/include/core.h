#ifndef CORE_H
#define CORE_H

#define _DEFAULT_SOURCE
#define _SVID_SOURCE
#define _BSD_SOURCE

#define BUF_SIZE 255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

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


void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd);
void free_fil(DIR_INFO** di);
void get_dir_info(char* path, DIR_INFO** di);

#endif
