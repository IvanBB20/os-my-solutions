#!/bin/bash

echo "graph_title SSA drive temperatures"
echo "graph_vlabel Celsius"
echo "graph_category sensors"
echo "graph_info  This graph shows SSA drive temp"


cat $1 | awk '$2 == "Array" {name=$3} \ 
              $5 == "Slot" {sl=$6} \
              $1 == "Array" {arr=$2} \ 
              $1 == "Unassigned" {arr="UN"} \
              $1 == "physicaldrive" {val=$2 ; print "SSA" sl name val ".label " "SSA" sl " " name " " sl " " val "\n" "SSA" sl name val ".type GAUGE"   }       
' | sed -E 's/(.*):(.*):(.*) /\1\2\3/g' 
