#!/usr/local/bin/bash
# prepare test file, output user and item column

# usage: prepare_test test_file

cat $1 | awk '$3==1{print $1 "\t" $2}' | sort | uniq > $1.ui

