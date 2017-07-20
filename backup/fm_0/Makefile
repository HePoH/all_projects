.PHONY: prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lncurses

SUBDIRS = lib src
OBJECTS = core.o graph.o main.o

all: prepare $(OBJECTS)
		#clear
		$(CC) -o fm $(OBJECTS) $(CFLAGS) $(LDFLAGS) -g
		./fm

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o fm

