#include "../include/core.h"
#include "../include/graph.h"

int main(){
	int ch;
	DIR_INFO *ld = NULL, *rd = NULL, *cd = NULL;

	init_core(&ld, &rd, &cd);
	init_graph();
	init_workspace(&ld, &rd);

	print_dir(ld, 1);
	print_dir(rd, 2);

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
					chdir(cd->path);
					print_dir(cd, 1);
				}
				break;

			case KEY_RIGHT:
				if (cd == ld) {
                                        print_dir(cd, 2);
                                        cd = rd;
					chdir(cd->path);
                                        print_dir(cd, 1);
                                }
                                break;

			case KEY_HOME:
				cd->current_file = cd->head_file;
                                print_dir(cd, 1);

                                break;

                        case KEY_END:
				cd->current_file = cd->tail_file;
                                print_dir(cd, 1);

				break;

			case 10:
				if (!chdir(cd->current_file->value.name)) {
					free_fil(&cd);
					get_dir_info(".", &cd);
					print_dir(cd, 1);
				}
				else
					perror("chdir");
				break;
		}
	}

	close_graph();

	free(ld);
	free(rd);
	cd = NULL;

	return 0;
}
