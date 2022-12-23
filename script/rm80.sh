#!/bin/sh

# Use this script to remove the 80 characters line break from files.
# It will always ask before removing line breaks. 
# If you want to edit this file, remember to put function calls inside round brackets '(' and ')' so that they are executed in a sub-shell and they don't modify variables outside of their scope

# regex for lines that SHOULD be modified, in BRE syntax
regex="^.+[^ .]
$"

# print line from file
pln() {
	file=$1
	line=$2 # line number

	sed -n "${line}p" $file
}

# show n lines before and after a certain line
showlns() {
	file=$1
	line=$2 # line number
	range=$3

	first=$(expr $line - $range)
	last=$(expr $line + $range)

	# bound checks
	[ $first -lt 1 ] && first=1
	[ $last -gt $(wc -l <$file) ] && last=$(wc -l <$file)

	# print lines
	for current in $(seq $first $last)
	do
		printf "$current | " >&2
		(pln $file $current) >&2
	done
}

# merge one line with the next one by replacing the line break in between with a space character
mergelns() {
	file=$1
	line=$2 # line number

	next=$(expr $line + 1)

	l1=$(pln $file $line) # current line
	l2=$(pln $file $next) # next line
	merge="$l1 $l2" # merge lines

	# replace $l1 and $l2 in $file with $merge
	# use a temporary variable so that the file does not end up empty (because of reading and writing on the same file I suppose)
	tmp=$(sed "${line}s/.*/$merge/" $file) # replace line with merge
	echo "$tmp" >$file
	tmp=$(sed "${next}d" $file) # delete next line
	echo "$tmp" >$file
}

# ask single occurrence in file at line
ask() {
	file=$1
	line=$2 # line number

	# print lines
	(showlns $file $line 2)
	
	while [ "$r" !=  "y" ] && [ "$r" != "n" ]
	do
		printf "remove line break at the end of line $line? [y/n] " >&2
		read r
	done

	[ "$r" = "y" ] && return 0

	return 1
}

# condition for asking, 0 on success
cond() {
	file=$1
	line=$2 # line number

	text=$(pln $file $line)

	length=$(echo $text | wc -m)

	# conditions
	[ $length -gt 1 ] && [ $length -le 80 ] && [ "$(echo "$text" | grep "$regex")" = "$text" ] && return 0

	return 1
}

# ask all occurrences in file
askall() {
	file=$1

	line=1
	lines=$(wc -l <$file)

	while [ $line -lt $lines ]
	do
		(cond $file $line) && (ask $file $line) && {
			# remove line break
			(mergelns $file $line)

			# update line count
			lines=$(wc -l <$file)

			continue
		}

		line=$(expr $line + 1)
	done
}

[ $# -eq 0 ] && { echo "$0: no files specified" >&2; exit 1; }

for f in $@
do
	echo "entering file $f" >&2
	(askall $f)
done
