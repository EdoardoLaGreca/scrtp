#! /bin/sh

pkg-config --exists "$1" >/dev/null 2>&1
exit $?
