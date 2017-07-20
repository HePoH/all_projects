#include "../include/core.h"

void init_core(DIR_INFO** ld, DIR_INFO** rd, DIR_INFO** cd) {
	*ld = get_dir_info(getenv("PWD"));
	*rd = get_dir_info(getenv("HOME"));
	*cd = *ld;
}

DIR_INFO* get_dir_info(char *path) {
	DIR_INFO* di = NULL;
	struct dirent **fnl;
	int fc;

	fc = scandir(path, &fnl, 0, alphasort);

	if (fc < 0)
		perror("scandir");
	else {
		int i = 0;

		di = (DIR_INFO*) malloc(fc * sizeof(DIR_INFO));
		getcwd(di->path, BUF_SIZE);
		di->file_count = fc - 2;

		if (fc > 0) {
			FILE_INFO_LIST* head = NULL;
			head = (FILE_INFO_LIST*) malloc(sizeof(FILE_INFO_LIST));

			strncpy(head->value.name, fnl[i]->d_name, BUF_SIZE - 1);
			strncpy(head->value.type, "-", BUF_SIZE - 1);

			head->value.size = 0;
			head->value.mod_date = 0;

			head->next = NULL;
			head->prev = NULL;

			di->head_file = head;
			di->current_file = head;

			i++;
		}

		for (; i < fc; i++) {
			//printf("%d - %s [%d] %d\n", fnl[i]->d_ino, fnl[i]->d_name, fnl[i]->d_type, fnl[i]->d_reclen);

			FILE_INFO_LIST* fil = NULL;
			fil = (FILE_INFO_LIST*) malloc(sizeof(FILE_INFO_LIST));

			strncpy(fil->value.name, fnl[i]->d_name, BUF_SIZE - 1);
			strncpy(fil->value.type, "-", BUF_SIZE - 1);

			fil->value.size = 0;
			fil->value.mod_date = 0;

			fil->next = NULL;
			fil->prev = di->current_file;

			di->current_file->next = fil;
			di->current_file = fil;

			free(fnl[i]);
		}

		di->current_file = di->head_file;
		free(fnl);
	}

	return di;
}
