.PHONY: clean #prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
#CFLAGS = -std=c99 -Wall -pedantic -ansi -g
CFLAGS =
LDFLAGS =

#SUBDIRS = lib src
OBJECTS = daemon.o client.o

all: $(OBJECTS)  #prepare $(OBJECTS)
                #clear
		$(CC) -o dm $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./dm

#prepare: $(SUBDIRS)
#$(SUBDIRS):
                #make -C $@

#src: lib

clean:
		clear
		rm -rf *.o time-daemon client fifo/dt.pipe fifo/time_daemon.pid log/time_daemon.log
