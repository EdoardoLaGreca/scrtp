#!/bin/sh

# insert required external tools here
reqbins=""

# log in stderr
eecho() {
	text=$1
	echo $text >&2
}

[ "$reqbins" ] && ../script/chkbins.sh $reqbins || exit 1

