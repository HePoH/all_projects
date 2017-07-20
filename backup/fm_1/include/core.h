#ifndef CORE_H
#define CORE_H

#define _SVID_SOURCE
#define BUF_SIZE 255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <curses.h>

typedef struct fi {
	char name[BUF_SIZE];
	char type[BUF_SIZE];
	int size;
	int mod_date;
} FILE_INFO;

typedef struct fil {
	FILE_INFO value;

	struct fil *next;
	struct fil *prev;
} FILE_INFO_LIST;

typedef struct di {
	char path[BUF_SIZE];
	int file_count;

	FILE_INFO_LIST* head_file;
	FILE_INFO_LIST* current_file;
	FILE_INFO_LIST* tail_file;

	int widt, height;
	int rows, cols;

	WINDOW *p_wnd, *m_wnd;
} DIR_INFO;


void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd);
void free_fil(DIR_INFO** di);
void get_dir_info(char* path, DIR_INFO** di);

#endif
