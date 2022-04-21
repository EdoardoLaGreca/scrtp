#! /bin/sh

for arg in "$@"
do
	output=`find /lib* /usr/lib* -name "lib$arg.so"`
	if [ ! "$output" ]
	then
		# library not found
		echo "library \"lib$arg.so\" not found"
		exit 1
	else
		# library found
		output=`echo $output | sed "s/^/\t/g"` # add tabs
		printf "library \"lib$arg.so\" found in:\n$output\n"
	fi
done

exit 0
