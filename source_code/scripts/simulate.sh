#!/bin/bash

./create_input.sh ../Logs/political_parties.txt 1000

cd ..

make run-poller &

sleep 1

make run-pollSwayer

kill -INT $(pidof poller)

cd scripts

./tallyVotes.sh ../Logs/tallyResultsFile

./processLogFile.sh ../Logs/log.txt

status1=$(diff ../Logs/pollerResultsFile ../Logs/tallyResultsFile)

status2=$(diff ../Logs/pollerResultsFile ../Logs/stat.txt)

if [ "$status1" == "" ] && [ "$status2" == "" ]; then
	echo -e "\033[0;32mTest passed\033[0m"
else
	echo -e "\033[0;31mTest failed\033[0m"
fi
