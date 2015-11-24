#!/bin/bash
VALGRINDOPTS1="--tool=memcheck --read-var-info=yes --read-inline-info=yes --track-origins=yes -v --track-fds=yes --leak-check=full"
VALGRINDOPTS2="--show-reachable=yes --log-file=valgrind.log --smc-check=all"
TESTCMD="./bin/32bit/Debug/traffic3_test -s"
#TESTCMD="./bin/32bit/Debug/traffic3 -o server -p 10000"
#TESTCMD="./bin/32bit/Debug/traffic3 -o client -p 10000"
valgrind  ${VALGRINDOPTS1} ${VALGRINDOPTS2} ${TESTCMD}
