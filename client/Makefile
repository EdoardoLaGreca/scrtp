.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3
OUTDIR = bin

all: checkbins shared

checkbins:
	../script/chkbins.sh $$(cat reqbins)

shared:
	(cd ../shared && make all)
	mkdir -p bin
	cp ../shared/bin/* ./bin

clean:
	rm -r $(OUTDIR)
