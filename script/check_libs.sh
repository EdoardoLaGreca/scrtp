#! /bin/sh

for arg in "$@"
	do
	if [ ! `find /lib* /usr/lib* -name "lib$arg.so"` ]
	then
		# library not found
		echo "library \"lib$arg.so\" not found"
		exit 1
	fi
done

exit 0
