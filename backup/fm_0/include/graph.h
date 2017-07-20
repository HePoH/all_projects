#ifndef GRAPH_H
#define GRAPH_H

#include <curses.h>
#include "core.h"

int rows, cols;

void init_graph();
void init_workspace(DIR_INFO** ld, DIR_INFO** rd);
void print_dir(DIR_INFO* di, int bg_c);
void close_graph();

#endif
