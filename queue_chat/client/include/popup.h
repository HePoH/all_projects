#ifndef POPUP_H
#define POPUP_H

#include "core.h"
#include "graph.h"

struct actions {
	char *key;
	int (*func)(void*);
};

void popup_new(int rows, int cols, int posy, int posx, int nb_buttons, char **requests, int nb_fields, char* title);
void popup_delete(void);
void popup_refresh(void);
bool popup_exists(void);
int popup_driver(void* args);

#endif
