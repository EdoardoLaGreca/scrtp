#! /bin/sh

ldconfig -p | grep "$1" >/dev/null 2>&1
exit $?
