#include "../include/core.h"
#include "../include/graph.h"
#include "../include/popup.h"

int main(){
	int ch = 0, cp_pth_ret[2], ep_st = 0, *cp_pth_st[2];
	char *file_path = NULL, *cur_dir = NULL;

	pid_t ep_pid = 0;
	pthread_t cp_tid = 0, cp_st_tid = 0;

	DIR_INFO *ld = NULL, *rd = NULL, *cd = NULL;
	COPY_FILE_INFO* cfi = NULL;

	init_core(&ld, &rd, &cd);
	init_graph();
	init_workspace(&ld, &rd);

	print_dir_static(cd);
	print_dir_static(rd);

	print_dir_dynamic(ld, 1);
	print_dir_dynamic(rd, 2);

	while((ch = getch()) != 'q') {
		switch(ch) {
			case KEY_UP:
				if (cd->current_file != cd->head_file) {
					cd->current_file = cd->current_file->prev;
					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_DOWN:
				if (cd->current_file->next != NULL) {
					cd->current_file = cd->current_file->next;
					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_LEFT:
				if (cd == rd) {
					print_dir_dynamic(cd, 2);
					cd = ld;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

					print_dir_dynamic(cd, 1);
				}
				break;

			case KEY_RIGHT:
				if (cd == ld) {
                                        print_dir_dynamic(cd, 2);
                                        cd = rd;

					if (chdir(cd->dir_path) == -1) {
						perror("chdir");
						return 0;
					}

                                        print_dir_dynamic(cd, 1);
                                }
                                break;

			case KEY_TAB:
				print_dir_dynamic(cd, 2);

				if (cd == ld)
					cd = rd;
				else
					cd = ld;

				if (chdir(cd->dir_path) == -1) {
					perror("chdir");
					return 0;
				}

				print_dir_dynamic(cd, 1);

				break;

			case KEY_HOME:
				cd->current_file = cd->head_file;
                                print_dir_dynamic(cd, 1);

                                break;

                        case KEY_END:
				cd->current_file = cd->tail_file;
                                print_dir_dynamic(cd, 1);

				break;

			case KEY_ENTER:
				if (!chdir(cd->current_file->file_name)) {
					free_fil(&cd);
					get_dir_info(".", &cd);

					print_dir_static(cd);
					print_dir_dynamic(cd, 1);
				}

				if (S_ISREG(cd->current_file->file_stat.st_mode))
					if (cd->current_file->file_stat.st_mode & S_IXUSR ||
					    cd->current_file->file_stat.st_mode & S_IXGRP ||
					    cd->current_file->file_stat.st_mode & S_IXOTH) {
						close_graph();

						ep_pid = fork();
						if (ep_pid == 0) {
							execl(cd->current_file->file_name, cd->current_file->file_name, NULL);
						}

						waitpid(ep_pid, &ep_st, WUNTRACED);

						init_graph();
						init_workspace(&ld, &rd);

						print_dir_dynamic(ld, 1);
						print_dir_dynamic(rd, 2);
					}

				break;

			case KEY_F(5):
				if (S_ISREG(cd->current_file->file_stat.st_mode)) {
					cfi = malloc(sizeof(COPY_FILE_INFO));
					if (cfi == NULL) {
						perror("malloc");
						exit(EXIT_FAILURE);
					}

					file_path = get_file_path(cd->current_file->file_name);
					cur_dir = get_file_path("");

					strncpy(cfi->fn_src, file_path, BUF_SIZE - 1);
					strncpy(cfi->fn_dst, cur_dir, BUF_SIZE - 1);

					if (display_copy_form(cfi)) {
						cp_pth_ret[0] = pthread_create(&cp_tid, NULL, copy_file, (void*)cfi);
						if(cp_pth_ret[0]) {
							perror("pthread_create");
							exit(EXIT_FAILURE);
						}

						usleep(1000);

						cp_pth_ret[1] = pthread_create(&cp_st_tid, NULL, display_copy_status, (void*)cfi);
						if(cp_pth_ret[1]) {
							perror("pthread_create");
							exit(EXIT_FAILURE);
						}

						pthread_join(cp_tid, (void**)&cp_pth_st[0]);
						pthread_join(cp_st_tid, (void**)&cp_pth_st[0]);
					}

					curs_set(0);

					free_fil(&cd);
					get_dir_info(".", &cd);

					wclear(ld->p_wnd);
					wclear(rd->p_wnd);

					box(ld->p_wnd, 0, 0);
					box(rd->p_wnd, 0, 0);

					wnoutrefresh(ld->p_wnd);
					wnoutrefresh(rd->p_wnd);

					doupdate();

					print_dir_static(ld);
					print_dir_static(rd);

					if (cd == ld) {
						print_dir_dynamic(ld, 1);
						print_dir_dynamic(rd, 2);
					}
					else {
						print_dir_dynamic(ld, 2);
						print_dir_dynamic(rd, 1);
					}

					free(file_path);
					free(cur_dir);
					free(cfi);

					file_path = NULL;
					cur_dir = NULL;
					cfi = NULL;
				}

				break;

			case KEY_F(8):
				if (S_ISREG(cd->current_file->file_stat.st_mode)) {
					file_path = get_file_path(cd->current_file->file_name);

					if (display_del_form(file_path))
						if (unlink(file_path) == -1)
							perror(unlink);

					curs_set(0);

					free_fil(&cd);
					get_dir_info(".", &cd);

					wclear(ld->p_wnd);
					wclear(rd->p_wnd);

					box(ld->p_wnd, 0, 0);
					box(rd->p_wnd, 0, 0);

					wnoutrefresh(ld->p_wnd);
					wnoutrefresh(rd->p_wnd);

					doupdate();

					print_dir_static(ld);
					print_dir_static(rd);

					if (cd == ld) {
						print_dir_dynamic(ld, 1);
						print_dir_dynamic(rd, 2);
					}
					else {
						print_dir_dynamic(ld, 2);
						print_dir_dynamic(rd, 1);
					}

					free(file_path);
					file_path = NULL;
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
