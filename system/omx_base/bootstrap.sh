#! /bin/sh

cd `dirname $0`

mkdir -p config && aclocal && automake --copy --add-missing --foreign && autoconf

