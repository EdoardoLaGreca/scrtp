all: client server

client:
	make -f client/Makefile all

server:
	make -f server/Makefile all

clean:
	rm -r bin/