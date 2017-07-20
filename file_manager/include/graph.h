#ifndef GRAPH_H
#define GRAPH_H

#include <curses.h>
#include <form.h>
#include <menu.h>

#include "core.h"

#define KEY_TAB 9
#define KEY_ENTER 10

void init_graph();
void init_workspace(DIR_INFO** ld, DIR_INFO** rd);
void print_dir_static(DIR_INFO* di);
void print_dir_dynamic(DIR_INFO* di, int bg_c);
int display_copy_form();
int display_del_form(char* del_file);
void close_graph();

#endif
