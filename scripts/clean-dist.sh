#!/bin/sh

[ -n "$1" ] || {
	echo "Please specify a directory"
	exit 1
}

[ -d $1 ] || {
	echo "You specified directory not exist"
	exit 1
}

for f in `find $1 -name *.gz`;do
	echo "Rename $f to ${f%.*}"
	mv $f ${f%.*}
done
