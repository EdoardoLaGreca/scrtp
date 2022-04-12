#! /bin/sh

ldconfig -p | grep "$1"
exit $?
