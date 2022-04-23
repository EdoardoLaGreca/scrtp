#! /bin/sh

cc=gcc
cflags="-std=c90 -Wall -Wpedantic -O3"
cflags_dbg="-std=c90 -Wall -Wpedantic -Og -g"
check_libs="../script/check_libs.sh"

# libraries to link with, without prefixes or suffixes (like "-l", "lib" or ".so")
libs="glfw GL"

# command to add "-l" to each library
gen_ld_args=`echo $libs | sed "s/^/-l/g" | sed "s/ / -l/g"`

# check dependencies
checkdeps() {
	echo "Checking dependencies..."
	$check_libs $libs
}

compile() {
	echo "Building scrtp..."
	mkdir -p obj
	ls *.c | sed "s/.c//" | xargs -I "{}" $cc $cflags -c -o obj/{}.o {}.c
}

compile_dbg() {
	echo "Building scrtp (debug)..."
	mkdir -p obj
	ls *.c | sed "s/.c//" | xargs -I "{}" $cc $cflags_dbg -c -o obj/{}.o {}.c
}

link() {
	name=$1
	echo "Linking scrtp..."
	mkdir -p bin
	$cc -o bin/$name obj/*.o $gen_ld_args
}

clean() {
	echo "Cleaning client directory..."
	rm -rf obj/
	rm -rf bin/
}

build() {
	compile
	link scrtp
}

build_dbg() {
	compile_dbg
	link scrtp_dbg
}

all() {
	checkdeps
	build
}

eval $1
