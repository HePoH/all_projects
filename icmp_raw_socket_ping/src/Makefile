CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS =

vpath %.h ../include/
vpath %.c ../lib/

ping.o: ping.c core.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

clean:                                                                                 
		rm -rf *.o *.out
