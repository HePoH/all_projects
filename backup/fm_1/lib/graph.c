#include "../include/core.h"
#include "../include/graph.h"

void init_graph() {
        if (initscr() == NULL) {
                perror("initscr");
                return;
        }

        noecho();
        curs_set(0);
        keypad(stdscr, true);
        cbreak();
        start_color();
        refresh();

        getmaxyx(stdscr, rows, cols);

	init_pair(1, COLOR_WHITE, COLOR_CYAN);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);
}

void init_workspace(DIR_INFO** ld, DIR_INFO** rd){
	(*ld)->p_wnd = newwin(rows, cols/2, 0 ,0);
        box((*ld)->p_wnd, '|', '-');
        (*ld)->m_wnd = derwin((*ld)->p_wnd, rows-2, cols/2-2, 1, 1);

        (*rd)->p_wnd = newwin(rows, cols/2, 0, cols/2);
        box((*rd)->p_wnd, '|', '-');
        (*rd)->m_wnd = derwin((*rd)->p_wnd, rows-2, cols/2-2, 1, 1);

	wrefresh((*ld)->p_wnd);
	wrefresh((*rd)->p_wnd);
}

void print_dir(DIR_INFO* di, int bg_c) {
	FILE_INFO_LIST* p = NULL;
	p = di->head_file;

	wclear(di->m_wnd);

    	box(di->p_wnd, '|', '-');
	wattron(di->p_wnd, A_BOLD);
	wattron(di->p_wnd, COLOR_PAIR(4));
	wmove(di->p_wnd, 0, cols / 4 - strlen(di->path) / 2);
        wprintw(di->p_wnd, "~ %s ~", di->path);
	wattroff(di->p_wnd, COLOR_PAIR(4));
    	wattroff(di->p_wnd, A_BOLD);

	wrefresh(di->p_wnd);

    	wattron(di->m_wnd, A_BOLD);
	while(p) {
		if (p == di->current_file) {
			wattron(di->m_wnd, COLOR_PAIR(bg_c));
			wprintw(di->m_wnd, " %s \n", p->value.name);
			wattroff(di->m_wnd, COLOR_PAIR(bg_c));
		}
		else {
			wattron(di->m_wnd, COLOR_PAIR(3));
			wprintw(di->m_wnd, " %s \n", p->value.name);
			wattroff(di->m_wnd, COLOR_PAIR(3));
		}

		p = p->next;
        }

	wattroff(di->m_wnd, A_BOLD);
        wprintw(di->m_wnd, "\n Count: %d\n", di->file_count);
	wrefresh(di->m_wnd);

	return;
}

void close_graph(){
	echo();
	endwin();
}
