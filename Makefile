MAKE_CLIENT = cd client && ./build.sh
MAKE_SERVER = cd server # add build.sh

all: client server

client:
	- chmod 744 client/build.sh
	$(MAKE_CLIENT) all

#server:
# todo

clean:
	- rm -r obj/
	- rm -r bin/
	$(MAKE_CLIENT) clean
	$(MAKE_SERVER) clean

.PHONY: all client server clean
