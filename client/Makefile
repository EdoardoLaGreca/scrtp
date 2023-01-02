.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3
OUTDIR = bin

all: checkbins readpkt mkpkt

checkbins:
	../script/chkbins.sh $$(cat reqbins)

readpkt: readpkt.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/readpkt readpkt.c

mkpkt: mkpkt.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/mkpkt mkpkt.c

clean:
	rm -r $(OUTDIR)
