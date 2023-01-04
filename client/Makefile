.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3
OUTDIR = bin

all: checkbins

checkbins:
	../script/chkbins.sh $$(cat reqbins)

clean:
	rm -r $(OUTDIR)
