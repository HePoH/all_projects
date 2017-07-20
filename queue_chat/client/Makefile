.PHONY: prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -std=c99 -O2 -Wall -pedantic -ansi -g
LDFLAGS = -lrt -lpthread -lncurses -lform -lmenu

SUBDIRS = lib src
#OBJECTS = core.o graph.o popup.o main.o
OBJECTS = core.o main.o

all: prepare $(OBJECTS)
		#clear
		$(CC) -o client $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./client

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o client

