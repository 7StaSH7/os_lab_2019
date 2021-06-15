CC=gcc
CFLAGS=-I.

all: client server
	
client: client.c factorial
	$(CC) client.c factorial.o -o client

server: server.c factorial
	$(CC) server.c factorial.o -lpthread  -o server

factorial: factorial.c factorial.h
	$(CC) -c factorial.c -o factorial.o

clean:
	rm factorial.o client server