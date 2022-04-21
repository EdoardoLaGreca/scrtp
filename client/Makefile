CC = gcc
CFLAGS = -std=c90 -Wall -Wpedantic -O3
CLINK = -lglfw -lGL
CHECKLIB = ../script/check_lib.sh

all: checkdeps build

build: compile link

# check dependencies
checkdeps:
	@ echo "Checking dependencies..."
	su -c "ldconfig" || echo "failed to update ldconfig, results may not be accurate"
	$(CHECKLIB) glfw
# add dependencies as `$(CHECKLIB) dep`

compile:
	@ echo "Building scrtp..."
	- mkdir obj
	ls | grep ".c$$" | sed "s/.c//" | xargs -I "{}" $(CC) $(CFLAGS) -c -o obj/{}.o {}.c

link:
	@ echo "Linking scrtp..."
	- mkdir bin
	$(CC) $(CLINK) -o bin/scrtp obj/*.o

clean:
	@ echo "Cleaning client directory..."
	- rm -r obj/
	- rm -r bin/

.PHONY: checkdeps clean
