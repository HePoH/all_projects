#include <ncurses.h>
#include <form.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static FORM *form;
static FIELD *fields[5];
static WINDOW *win_body, *win_form;

int cols = 0, rows = 0;

static char* trim_whitespaces(char *str) {
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

static void driver(int ch) {
	int i;

	switch (ch) {
		case KEY_F(2):
			// Or the current field buffer won't be sync with what is displayed
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_PREV_FIELD);
			move(LINES-3, 2);

			for (i = 0; fields[i]; i++) {
				printw("%s", trim_whitespaces(field_buffer(fields[i], 0)));

				if (field_opts(fields[i]) & O_ACTIVE)
					printw("\"\t");
				else
					printw(": \"");
			}

			refresh();
			pos_form_cursor(form);
			break;

		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			form_driver(form, REQ_END_LINE);
			break;

		case KEY_LEFT:
			form_driver(form, REQ_PREV_CHAR);
			break;

		case KEY_RIGHT:
			form_driver(form, REQ_NEXT_CHAR);
			break;

		// Delete the char before cursor
		case KEY_BACKSPACE:
		case 127:
			form_driver(form, REQ_DEL_PREV);
			break;

		// Delete the char under the cursor
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;

		default:
			form_driver(form, ch);
			break;
	}

	wrefresh(win_form);
}

int main() {
	int ch;

	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, rows, cols);

	win_body = newwin(12, cols / 2, rows / 2 - 6, cols / 2 - cols / 4);
	assert(win_body != NULL);
	box(win_body, 0, 0);
	win_form = derwin(win_body, 10, cols / 2 - 2, 1, 1);
	assert(win_form != NULL);
	box(win_form, 0, 0);
	mvwprintw(win_body, 0, cols / 4 - 6, "[ Copying ]");

	/*FIELD *new_field(int height, int width, int toprow, int leftcol, int offscreen, int nbuffers);*/

	fields[0] = new_field(1, 10, 0, 1, 0, 0);
	fields[1] = new_field(1, cols / 2 - 6, 2, 1, 0, 0);
	fields[2] = new_field(1, 10, 4, 1, 0, 0);
	fields[3] = new_field(1, cols / 2 - 6, 6, 1, 0, 0);
	fields[4] = NULL;
	assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);

	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);

	set_field_buffer(fields[0], 0, "[ From ]:");
	set_field_buffer(fields[1], 0, "/home/2017/d_medvedev/work/file_manager");
	set_field_buffer(fields[2], 0, "[ To ]:");
	set_field_buffer(fields[3], 0, "/home/2017/d_medvedev/work/file_manager");

	set_field_back(fields[1], COLOR_PAIR(1));
	set_field_fore(fields[1], COLOR_PAIR(1));

	set_field_back(fields[3], COLOR_PAIR(1));
	set_field_fore(fields[3], COLOR_PAIR(1));

	set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
	set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
	set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[3], A_UNDERLINE);

	form = new_form(fields);
	assert(form != NULL);
	set_form_win(form, win_form);
	set_form_sub(form, derwin(win_form, 8, cols / 2 - 4, 1, 1));
	post_form(form);

	refresh();
	wrefresh(win_body);
	wrefresh(win_form);

	while ((ch = getch()) != KEY_F(1))
		driver(ch);

	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
	free_field(fields[2]);
	free_field(fields[3]);
	delwin(win_form);
	delwin(win_body);
	endwin();

	return 0;
}
