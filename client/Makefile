CC = gcc
CFLAGS = -std=c90 -Wall -Wpedantic -O3
CHECKLIB = ../script/check_lib.sh

all: checkdeps build link

# check dependencies
checkdeps:
	@ echo "Checking dependencies..."
	su -c "ldconfig" || echo "failed to update ldconfig, results may not be accurate"
	$(CHECKLIB) glfw
# add dependencies as `$(CHECKLIB) dep`

build:
	@ echo "Building..."
# add build objects

link:
	@ echo "Linking..."

clean:
	@ echo "Cleaning client directory..."
	- rm -r obj/
	- rm -r bin/
