#!/bin/bash

USAGE="$0 <input> <output> <executable>"

if [ -z "$3" ] ; then
	echo "$USAGE"
	exit 1
fi

INPUT="$1"
OUTPUT="$2"
EXE="$3"
NUM_CPU=$(cat /proc/cpuinfo | grep -c processor)

if [ -n "$(file $INPUT | grep ERROR)" ] ; then
	echo "invalid input file"
	exit 1
fi

rm -f $OUTPUT

for i in $(output $INPUT) ; do
	echo "$(echo \"$i\" | $EXE) $i" >> $OUTPUT
done

