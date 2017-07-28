IDIR = include
LDIR = lib
SDIR = src
LFLAGS = -lpthread
CFLAGS = -g -O2 -Wall -pedantic -ansi
CC = gcc

SUBDIRS = $(LDIR) $(SDIR)
OBJMAIN = $(SDIR)/main.o $(LDIR)/core.o

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: main

main: $(OBJMAIN)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) 
		mv main ~/sudo/snif

$(OBJMAIN): prepare

.PHONY: prepare $(SUBDIRS)
prepare: $(SUBDIRS)

$(SUBDIRS):
		make -C $@
		
$(SDIR): $(LDIR)

clean:
		clear
		cd src	&& $(MAKE) clean
		cd lib	&& $(MAKE) clean
		rm -rf main   *.o
		rm -rf ~/sudo/snif
