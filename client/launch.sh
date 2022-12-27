#!/bin/sh

# insert required external tools here
reqbins=""

# log in stderr
eecho() {
	text=$1
	echo $text >&2
}

../script/chkbins.sh $reqbins || { eecho "$0: missing tools"; exit 1; }

