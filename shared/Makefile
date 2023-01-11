.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3

checkbins:
	./chkbins.sh $$(cat reqbins)

all: bin/readpkt bin/mkpkt bin/tohex

bin/readpkt: readpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/readpkt readpkt.c

bin/mkpkt: mkpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/mkpkt mkpkt.c

bin/tohex: tohex.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/tohex tohex.c

clean:
	rm -r bin
