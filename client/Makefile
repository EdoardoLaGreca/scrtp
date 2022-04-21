CC = gcc
CFLAGS = -std=c90 -Wall -Wpedantic -O3
CHECK_LIBS = ../script/check_libs.sh

# libraries to link with, without prefixes or suffixes (like "-l", "lib" or ".so")
LIBS = glfw GL

# command to add "-l" to each library
GEN_LD_ARGS = echo $(LIBS) | sed "s/^/-l/g" | sed "s/ / -l/g"

all: checkdeps build

build: compile link

# check dependencies
checkdeps:
	@ echo "Checking dependencies..."
	$(CHECKLIB) $(LIBS)

compile:
	@ echo "Building scrtp..."
	- mkdir obj
	ls | grep ".c$$" | sed "s/.c//" | xargs -I "{}" $(CC) $(CFLAGS) -c -o obj/{}.o {}.c

link:
	@ echo "Linking scrtp..."
	- mkdir bin
	$(CC) `$(GEN_LD_ARGS)` -o bin/scrtp obj/*.o

clean:
	@ echo "Cleaning client directory..."
	- rm -r obj/
	- rm -r bin/

.PHONY: checkdeps clean
