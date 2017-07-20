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

	while((ch = getchar()) != 'q') {
		switch(ch) {
			case KEY_UP: break;
			case KEY_DOWN: break;
			case KEY_LEFT: break;
			case KEY_RIGHT: break;
		}
	}

	getchar();

	close_graph();
	return 0;
}
