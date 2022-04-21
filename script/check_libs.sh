#! /bin/sh

if [ `find /lib* /usr/lib* -iname "lib$1.so"` ]
then
  exit 0
else
  exit 1
fi
