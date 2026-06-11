#!/bin/sh
export -n $(printenv | cut -d= -f1)
export PATH=/bin:/usr/local/bin/:/usr/local/opt/make/libexec/gnubin/:/usr/bin
make
