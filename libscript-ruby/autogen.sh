#!/bin/sh

if [ "$1" = "--clean" ]
then
   rm -rf *~ autom4te.cache libltdl depcomp missing install-sh install.sh
   rm -rf ltmain.sh configure config.sub config.log config.guess
   rm -rf config.h.in aclocal.m4 Makefile.in
   exit 0
fi

aclocal -I m4
autoheader
autoconf
libtoolize --automake --copy
automake --add-missing --copy
