.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3
OUTDIR = bin

all: readpkt

readpkt: readpkt.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/readpkt readpkt.c

sendraw: sendraw.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/sendraw sendraw.c

clean:
	rm -r $(OUTDIR)
