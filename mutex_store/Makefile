SUBDIRS = lib src
OBJECTS = ms.o main.o

.PHONY: clean prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -g 
LDFLAGS = -lpthread

all: $(OBJECTS) prepare $(OBJECTS)
		#clear
		$(CC) -o ms $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./spt

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o ms
