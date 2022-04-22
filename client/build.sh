#! /bin/sh

cc=gcc
cflags="-std=c90 -Wall -Wpedantic -O3"
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
	ls | grep ".c$$" | sed "s/.c//" | xargs -I "{}" $cc $cflags -c -o obj/{}.o {}.c
}

link() {
	echo "Linking scrtp..."
	mkdir -p bin
	$cc $gen_ld_args -o bin/scrtp obj/*.o
}

clean() {
	echo "Cleaning client directory..."
	rm -rf obj/
	rm -rf bin/
}

build() {
	compile
	link
}

all() {
	checkdeps
	build
}

case $1 in
	"checkdeps")
		checkdeps
		;;
	"compile")
		compile
		;;
	"link")
		link
		;;
	"clean")
		clean
		;;
	"build")
		build
		;;
	"all")
		all
		;;
	*)
		echo "Usage: $0 [checkdeps|compile|link|clean|build|all]"
		;;
esac
