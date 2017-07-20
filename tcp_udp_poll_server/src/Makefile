CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS =

vpath %.h ../include/
vpath %.c ../lib/

server.o: server.c server_core.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

tcp_client.o: tcp_client.c client_core.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

udp_client.o: udp_client.c client_core.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

clean:                                                                                 
		rm -rf *.o *.out
