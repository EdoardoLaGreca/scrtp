#! /bin/sh

for arg in "$@"
	do
	if [ ! `find /lib* /usr/lib* -iname "lib$1.so"` ]
	then
		# library not found
		echo "library $1 not found"
		exit 1
	fi
done

exit 0
