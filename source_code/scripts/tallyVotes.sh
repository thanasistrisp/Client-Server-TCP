#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <file>"
	exit 1
fi

if [ -r inputFile ]; then
	inputFile=inputFile
elif [ -r ../Logs/inputFile ]; then
	inputFile=../Logs/inputFile
else
	echo "File $1 does not exist or is not readable"
	exit 1
fi

> $1

awk '!seen[$1,$2]++ {print $3}' $inputFile > /tmp/count_votes.txt
sed -i '/^$/d' /tmp/count_votes.txt

sort /tmp/count_votes.txt | uniq -c | sort -n | awk '{print $2, $1}' > /tmp/count_votes_1.txt

sort -k1 /tmp/count_votes_1.txt > $1

echo "TOTAL $(wc -l < /tmp/count_votes.txt)" >> $1

rm -f /tmp/count_votes.txt /tmp/count_votes_1.txt
