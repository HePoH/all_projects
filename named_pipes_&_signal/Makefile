.PHONY: clean #prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
#CFLAGS = -std=c99 -Wall -pedantic -ansi -g
CFLAGS =
LDFLAGS =

#SUBDIRS = lib src
OBJECTS = server.o client.o

all: $(OBJECTS)  #prepare $(OBJECTS)
                #clear
		$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		#./nps

#prepare: $(SUBDIRS)
#$(SUBDIRS):
                #make -C $@

#src: lib

clean:
		clear
		rm -rf *.o server client fs_pid
