#! /bin/sh

ldconfig -p | grep "lib$1.so" >/dev/null 2>&1
exit $?
