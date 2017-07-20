.PHONY: clean #prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -ansi -g
LDFLAGS =

#SUBDIRS = lib src
OBJECTS = main.o

all: $(OBJECTS)  #prepare $(OBJECTS)
                #clear
		$(CC) -o up $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./up

#prepare: $(SUBDIRS)
#$(SUBDIRS):
                #make -C $@

#src: lib

clean:
		clear
		rm -rf *.o up
