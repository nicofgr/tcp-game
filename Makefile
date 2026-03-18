CC = clang
CFLAGS = -Wall

all: server client

server:
	$(CC) $(CFLAGS) server.c -o server.out

client: 
	$(CC) $(CFLAGS) client.c -o client.out

