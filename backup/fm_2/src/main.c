#include "../include/core.h"
#include "../include/graph.h"

int main(){
	int ch;
	pid_t pid = 0;
	DIR_INFO *ld = NULL, *rd = NULL, *cd = NULL;

	init_core(&ld, &rd, &cd);
	init_graph();
	init_workspace(&ld, &rd);

	print_dir(ld, 1);
	print_dir(rd, 2);

	print_dir_static(cd);
	print_dir_static(rd);

	while((ch = getch()) != 'q') {
		switch(ch) {
			case KEY_UP:
				if (cd->current_file != cd->head_file) {
					cd->current_file = cd->current_file->prev;
					print_dir(cd, 1);
				}
				break;

			case KEY_DOWN:
				if (cd->current_file->next != NULL) {
					cd->current_file = cd->current_file->next;
					print_dir(cd, 1);
				}
				break;

			case KEY_LEFT:
				if (cd == rd) {
					print_dir(cd, 2);
					cd = ld;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

					print_dir(cd, 1);
				}
				break;

			case KEY_RIGHT:
				if (cd == ld) {
                                        print_dir(cd, 2);
                                        cd = rd;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

                                        print_dir(cd, 1);
                                }
                                break;

			case KEY_TAB:
				print_dir(cd, 2);

				if (cd == ld)
					cd = rd;
				else
					cd = ld;

				if (chdir(cd->dir_path) == -1) {
					perror("chdir");
					return 0;
				}

				print_dir(cd, 1);

				break;

			case KEY_HOME:
				cd->current_file = cd->head_file;
                                print_dir(cd, 1);

                                break;

                        case KEY_END:
				cd->current_file = cd->tail_file;
                                print_dir(cd, 1);

				break;

			case KEY_ENTER:
				if (!chdir(cd->current_file->file_name)) {
					free_fil(&cd);
					get_dir_info(".", &cd);
					print_dir(cd, 1);
					print_dir_static(cd);
				}

				if (S_ISREG(cd->current_file->file_stat.st_mode))
					if (cd->current_file->file_stat.st_mode & S_IXUSR ||
					    cd->current_file->file_stat.st_mode & S_IXGRP ||
					    cd->current_file->file_stat.st_mode & S_IXOTH) {
						close_graph();

						pid = fork();
						if (pid == 0) {
							execl(cd->current_file->file_name, cd->current_file->file_name, (char *) 0);
						}

						wait(0);

						init_graph();
						init_workspace(&ld, &rd);

						print_dir(ld, 1);
						print_dir(rd, 2);
					}

				break;
		}
	}

	close_graph();

	free(ld);
	free(rd);
	cd = NULL;

	exit(EXIT_SUCCESS);
}
