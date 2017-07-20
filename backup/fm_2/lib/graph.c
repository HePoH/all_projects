#include "../include/core.h"
#include "../include/graph.h"

void init_graph() {
        if (initscr() == NULL) {
                perror("initscr");
                exit(EXIT_FAILURE);
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
	/*
		x - cols
		y - rows
	*/

	(*ld)->p_wnd = newwin(rows, cols/2, 0, 0);
        box((*ld)->p_wnd, '|', '-');

	(*ld)->fn_wnd = derwin((*ld)->p_wnd, rows-2, cols/2 - 48, 1, 2);
	(*ld)->ow_wnd = derwin((*ld)->p_wnd, rows-2, 13, 1, cols/2 - 44);
	(*ld)->s_wnd = derwin((*ld)->p_wnd, rows-2, 9, 1, cols/2 - 31);
	(*ld)->mt_wnd = derwin((*ld)->p_wnd, rows-2, 13, 1, cols/2 - 20);
	(*ld)->pr_wnd = derwin((*ld)->p_wnd, rows-2, 4, 1, cols/2 - 5);

        (*rd)->p_wnd = newwin(rows, cols/2, 0, cols/2);
        box((*rd)->p_wnd, '|', '-');

	(*rd)->fn_wnd = derwin((*rd)->p_wnd, rows-2, cols/2 - 48, 1, 2);
	(*rd)->ow_wnd = derwin((*rd)->p_wnd, rows-2, 13, 1, cols/2 - 44);
	(*rd)->s_wnd = derwin((*rd)->p_wnd, rows-2, 9, 1, cols/2 - 31);
	(*rd)->mt_wnd = derwin((*rd)->p_wnd, rows-2, 13, 1, cols/2 - 20);
	(*rd)->pr_wnd = derwin((*rd)->p_wnd, rows-2, 4, 1, cols/2 - 5);

	wrefresh((*ld)->p_wnd);
	wrefresh((*rd)->p_wnd);
}

void print_dir_static(DIR_INFO* di) {
	mvwhline(di->p_wnd, 0, 1, '-', cols/2 - 2);
	mvwhline(di->p_wnd, rows - 1, 1, '-', cols/2 - 2);

	wattron(di->p_wnd, A_BOLD);
	wattron(di->p_wnd, COLOR_PAIR(4));

	wmove(di->p_wnd, 0, cols / 4 - strlen(di->dir_path) / 2 - 3);
        wprintw(di->p_wnd, "[ %s ]", di->dir_path);

	wmove(di->p_wnd, rows - 1, cols / 8 - 8);
        wprintw(di->p_wnd, "[ Total: %d ]", di->file_count);

	wattroff(di->p_wnd, COLOR_PAIR(4));
    	wattroff(di->p_wnd, A_BOLD);

	mvwvline(di->p_wnd, 1, cols/2 - 46, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 33, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 22, '|', rows-2);
	mvwvline(di->p_wnd, 1, cols/2 - 7, '|', rows-2);

	mvwhline(di->p_wnd, 2, 1, '-', cols/2 - 2);

	wrefresh(di->p_wnd);
}

void print_dir(DIR_INFO* di, int bg_c) {
	FILE_INFO_LIST* p = NULL;
	struct tm* timeinfo = NULL;
	char mt_buffer[BUF_SIZE];

	p = di->head_file;

	wclear(di->fn_wnd);
	wclear(di->ow_wnd);
	wclear(di->s_wnd);
	wclear(di->mt_wnd);
	wclear(di->pr_wnd);

    	wattron(di->fn_wnd, A_BOLD);
    	wattron(di->ow_wnd, A_BOLD);
    	wattron(di->s_wnd, A_BOLD);
    	wattron(di->mt_wnd, A_BOLD);
    	wattron(di->pr_wnd, A_BOLD);

	wattron(di->fn_wnd, COLOR_PAIR(3));
	wattron(di->ow_wnd, COLOR_PAIR(3));
	wattron(di->s_wnd, COLOR_PAIR(3));
	wattron(di->mt_wnd, COLOR_PAIR(3));
	wattron(di->pr_wnd, COLOR_PAIR(3));

	wprintw(di->fn_wnd, "File name\n\n");
	wprintw(di->ow_wnd, "Owner\n\n");
	wprintw(di->s_wnd, "Size\n\n");
	wprintw(di->mt_wnd, "Mod time\n\n");
	wprintw(di->pr_wnd, "Prm\n\n");

	wattroff(di->fn_wnd, COLOR_PAIR(3));
	wattroff(di->ow_wnd, COLOR_PAIR(3));
	wattroff(di->s_wnd, COLOR_PAIR(3));
	wattroff(di->mt_wnd, COLOR_PAIR(3));
	wattroff(di->pr_wnd, COLOR_PAIR(3));

	while(p) {
		struct passwd* pwuser;
		timeinfo = localtime(&p->file_stat.st_mtim.tv_sec);
		strftime (mt_buffer, BUF_SIZE, "%b %e %R%n", timeinfo);

		if ((pwuser = getpwuid(p->file_stat.st_uid)) == NULL){
			perror("getpwuid()");
			exit(EXIT_FAILURE);
		}

		if (p == di->current_file) {
			wattron(di->fn_wnd, COLOR_PAIR(bg_c));
			wattron(di->ow_wnd, COLOR_PAIR(bg_c));
			wattron(di->s_wnd, COLOR_PAIR(bg_c));
			wattron(di->mt_wnd, COLOR_PAIR(bg_c));
			wattron(di->pr_wnd, COLOR_PAIR(bg_c));

			wprintw(di->fn_wnd, "%s\n", p->file_name, p->file_stat.st_uid, pwuser->pw_name);
			wprintw(di->ow_wnd, "%s\n", pwuser->pw_name, p->file_stat.st_uid);
			wprintw(di->s_wnd, "%ld\n", p->file_stat.st_size);
			wprintw(di->mt_wnd, "%s", mt_buffer);
			wprintw(di->pr_wnd, "%o\n", p->file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

			wattroff(di->fn_wnd, COLOR_PAIR(bg_c));
			wattroff(di->ow_wnd, COLOR_PAIR(bg_c));
			wattroff(di->s_wnd, COLOR_PAIR(bg_c));
			wattroff(di->mt_wnd, COLOR_PAIR(bg_c));
			wattroff(di->pr_wnd, COLOR_PAIR(bg_c));
		}
		else {
			wattron(di->fn_wnd, COLOR_PAIR(3));
			wattron(di->ow_wnd, COLOR_PAIR(3));
			wattron(di->s_wnd, COLOR_PAIR(3));
			wattron(di->mt_wnd, COLOR_PAIR(3));
			wattron(di->pr_wnd, COLOR_PAIR(3));

			wprintw(di->fn_wnd, "%s\n", p->file_name);
			wprintw(di->ow_wnd, "%s\n", pwuser->pw_name, p->file_stat.st_uid);
			wprintw(di->s_wnd, "%ld\n", p->file_stat.st_size);
			wprintw(di->mt_wnd, "%s", mt_buffer);
			wprintw(di->pr_wnd, "%o\n", p->file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

			wattroff(di->fn_wnd, COLOR_PAIR(3));
			wattroff(di->ow_wnd, COLOR_PAIR(3));
			wattroff(di->s_wnd, COLOR_PAIR(3));
			wattroff(di->mt_wnd, COLOR_PAIR(3));
			wattroff(di->pr_wnd, COLOR_PAIR(3));
		}

		p = p->next;
        }

	wattroff(di->fn_wnd, A_BOLD);
	wattroff(di->ow_wnd, A_BOLD);
	wattroff(di->s_wnd, A_BOLD);
	wattroff(di->mt_wnd, A_BOLD);
	wattroff(di->pr_wnd, A_BOLD);

	wrefresh(di->fn_wnd);
	wrefresh(di->ow_wnd);
	wrefresh(di->s_wnd);
	wrefresh(di->mt_wnd);
	wrefresh(di->pr_wnd);

	print_dir_static(di);
}

void close_graph(){
	echo();
	endwin();
}
