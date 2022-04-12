MAKE_CLIENT = make -f client/Makefile
MAKE_SERVER = make -f server/Makefile

all: client server

client:
	$(MAKE_CLIENT) all

server:
	$(MAKE_SERVER) all

clean:
	- rm -r obj/
	- rm -r bin/
	$(MAKE_CLIENT) clean
	$(MAKE_SERVER) clean
