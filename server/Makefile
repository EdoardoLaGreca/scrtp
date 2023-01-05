.POSIX:

CC = gcc
CFLAGS = -std=c89 -Wall -Wpedantic -O3

# things to do after compilation
post: checkbins shared

checkbins:
	./chkbins.sh $$(cat reqbins)

shared:
	(cd ../shared && make)
	mkdir -p bin
	cp ../shared/bin/* bin
	cp ../shared/common.sh bin
	cp ../shared/chkbins.sh bin
	echo " $$(cat ../shared/reqbins)" >>reqbins



clean:
	rm -r bin
