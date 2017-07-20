.PHONY: prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -std=c99 -O2 -Wall -pedantic -ansi
LDFLAGS = -lncurses -lform

SUBDIRS = lib src
OBJECTS = core.o graph.o main.o

all: prepare $(OBJECTS)
		#clear
		$(CC) -o fm $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./fm

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o fm

