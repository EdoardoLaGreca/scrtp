MAKE_CLIENT = cd client && build.sh
MAKE_SERVER = cd server # add build.sh

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

.PHONY: all client server clean
