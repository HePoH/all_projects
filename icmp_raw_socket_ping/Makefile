IDIR = include
LDIR = lib
SDIR = src
LFLAGS = -lpthread
CFLAGS = -g -O2 -Wall -pedantic -ansi
CC = gcc

SUBDIRS = $(LDIR) $(SDIR)
OBJMAIN = $(SDIR)/ping.o $(LDIR)/core.o

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: ping

ping: $(OBJMAIN)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

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
		rm -rf ping *.o
