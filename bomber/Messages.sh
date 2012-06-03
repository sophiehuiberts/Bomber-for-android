#! /bin/sh
# $EXTRACTRC `find -name \*.ui` >> rc.cpp || exit 11
$EXTRACTRC `find . -name "*.rc"` >> rc.cpp || exit 12
$EXTRACTRC `find . -name "*.kcfg"` >> rc.cpp
$XGETTEXT rc.cpp `find . -name '*.cpp'` -o $podir/bomber.pot
rm -f rc.cpp
