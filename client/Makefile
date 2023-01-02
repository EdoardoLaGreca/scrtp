.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3
OUTDIR = bin

# insert required external tools here
REQBINS = nc openssl

all: checkbins readpkt mkpkt

checkbins:
	../script/chkbins.sh $(REQBINS)

readpkt: readpkt.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/readpkt readpkt.c

mkpkt: mkpkt.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/mkpkt mkpkt.c

clean:
	rm -r $(OUTDIR)
