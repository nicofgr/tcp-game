CC = clang
CFLAGS = -Wall -std=c99 -fsanitize=address

all:
	$(CC) $(CFLAGS) main.c -o saida.out


