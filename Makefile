OBJS=reflect_server.o reflect_client.o
CC=gcc
XX=g++
CFLAGS=-Wall
#CXXFLAGS=-Wall -std -std=c++11

build:$(OBJS)
	$(CC) reflect_server.o -o reflect_server
	$(CC) reflect_client.o -o reflect_client

reflect_server.o:reflect_server.c
	$(CC) $(CFLAGS) -c reflect_server.c -o reflect_server.o

reflect_client.o:reflect_client.c
	$(CC) $(CFLAGS) -c reflect_client.c -o reflect_client.o

.PHONY : clean clean_objs

clean:
	-rm -rf *.o reflect_server reflect_client

clean_objs:
	-rm -rf *.o

