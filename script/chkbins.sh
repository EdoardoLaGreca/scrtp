#!/bin/sh

# Check existence of required binaries in the current system.

check() {
	name=$1

	IFS=':'
	for dir in $PATH
	do
		# check that $dir/$name exists AND is executable
		test -x $dir/$name && return 0
	done

	return 1
}

[ $# -lt 2 ] && { echo "$0: no binaries to check" >&2 ; exit 0 ; }

iserr="no"

for arg in $@
do
	check $arg
	[ $? -gt 0 ] && { iserr="yes" ; echo "$0: $arg: binary not found in PATH" >&2 ; }
done

[ iserr = "yes" ] && exit 1

exit 0
