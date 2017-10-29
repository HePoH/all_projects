IDIR = include
LDIR = lib
SDIR = src

CC = gcc
LFLAGS = -lpthread -lpcap
CFLAGS = -g -O2 -Wall -pedantic -ansi

SUBDIRS = $(LDIR) $(SDIR)
OBJMAIN = $(SDIR)/sniffer.o $(LDIR)/core.o

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: sniffer

sniffer: $(OBJMAIN)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

$(OBJSERV): prepare

.PHONY: prepare $(SUBDIRS)
prepare: $(SUBDIRS)

$(SUBDIRS):
		make -C $@

$(SDIR): $(LDIR)

clean:
		clear
		cd src	&& $(MAKE) clean
		cd lib	&& $(MAKE) clean
		rm -rf sniffer *.o
