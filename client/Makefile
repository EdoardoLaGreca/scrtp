CC = gcc
CFLAGS = -std=c90 -Wall -Wpedantic -O3
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
	@ echo "Building..."
	- mkdir obj
	ls | grep ".c$" | xargs -I "{}" $(CC) $(CFLAGS) -co "obj/{}"

link:
	@ echo "Linking..."
	- mkdir bin
	$(CC) -o bin/client obj/*.o

clean:
	@ echo "Cleaning client directory..."
	- rm -r obj/
	- rm -r bin/
