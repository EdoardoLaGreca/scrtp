.POSIX:

all: client server

client:
	(cd client && make all)

server:
	# (cd server && make all)

clean:
	rm -r client/bin
	# rm -r server/bin
