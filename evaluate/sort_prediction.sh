#!/usr/local/bin/bash
# sort prediction, output user and item column

# usage: sort_prediction pred_file

cat $1 | sort -k3gr | awk '{print $1 "\t" $2}' > $1.ui

