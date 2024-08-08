#!/bin/bash

if [ $# -ne 2 ]; then
	echo "Usage: $0 <file> <numLines>"
	exit 1
fi

if [ ! -r $1 ]; then
	echo "File $1 does not exist or is not readable"
	exit 1
fi

> ../Logs/inputFile

i=$2
while [ $i -gt 0 ];
do
	rand=$(( ( RANDOM % 10 )  + 3 ))
	name=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $rand | head -n 1)
	surname=$(cat /dev/urandom | tr -dc 'a-zA-Z' | fold -w $rand | head -n 1)
	line=$(shuf -n 1 ../Logs/$1)
	echo "$name $surname $line" >> ../Logs/inputFile
	i=$(( $i - 1 ))
done
