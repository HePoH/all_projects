#ifndef CMPLX_NUM_H
#define CMPLX_NUM_H

#define _SVID_SOURCE
#define BUF_SIZE 255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include <dlfcn.h>

typedef struct cmplx {
	double a;
	double b;
} cmplx_num;

#endif
