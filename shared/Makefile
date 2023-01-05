.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3

checkbins:
	./chkbins.sh $$(cat reqbins)

bin/readpkt: readpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/readpkt readpkt.c

bin/mkpkt: mkpkt.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/mkpkt mkpkt.c

clean:
	rm -r bin
