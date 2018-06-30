0 = parse.o
all = nsh
CC = gcc
CCFLAGS = -ggdb
nsh: wayne_parse.o main-parse.o
	$(CC) $(CFLAGS) -o nsh wayne_parse.o main-parse.o
main-parse.o: main-parse.c
	$(CC) -c $(CFLAGS) -std=c99 main-parse.c
wayne_parse.o: wayne_parse.h wayne_parse.c
	$(CC) -c $(CFLAGS) -std=c99 wayne_parse.c
clean:
	/bin/rm -f nsh *.o

