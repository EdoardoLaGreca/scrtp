#! /bin/sh

for arg in "$@"
	do
	if [ ! `find /lib* /usr/lib* -name "lib$1.so"` ]
	then
		# library not found
		echo "library \"lib$1.so\" not found"
		exit 1
	fi
done

exit 0
