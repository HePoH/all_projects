SUBDIRS = lib src
OBJECTS = main.o

.PHONY: prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib
#vpath %.so plugins

CC = gcc
CFLAGS = -std=c99 -O2 -Wall -pedantic -ansi
LDFLAGS = -ldl

all: prepare $(OBJECTS)
		#clear
		$(CC) -o calc $(OBJECTS) $(CFLAGS) $(LDFLAGS) -g
		./calc

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o calc
		rm -rf lib/*.o plugins/*.so
