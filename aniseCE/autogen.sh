#!/bin/sh

aclocal
automake --add-missing --copy
autoconf

echo "Now you are ready to run './configure && make' command."
