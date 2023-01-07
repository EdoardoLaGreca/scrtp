.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3

checkbins:
	./chkbins.sh $$(cat reqbins)

all: bin/readpkt bin/mkpkt bin/str2h

bin/readpkt: readpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/readpkt readpkt.c

bin/mkpkt: mkpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/mkpkt mkpkt.c

bin/str2h: str2h.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/str2h str2h.c

clean:
	rm -r bin
