all: ssocks

CC=g++
CFLAGS=-ggdb -c
LIBS=-lpthread

ssocks: client.o ipfuncs.o main.o server.o
	$(CC) $(LIBS) client.o ipfuncs.o main.o server.o -o ssocks

client.o: client.cpp
	 $(CC) $(CFLAGS) client.cpp

ipfuncs.o: ipfuncs.cpp
	$(CC) $(CFLAGS) ipfuncs.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

clean:
	rm -rf *o ssocks
