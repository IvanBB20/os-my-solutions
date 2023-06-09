#!/bin/bash

if [ $# -ne 2 ]
then
        echo "Wrong number of args"
        exit 1
fi 

if [ ! -f ${1} ]
then
    echo "First argument is not a file"
    exit 2
fi

if [ ! -d ${2} ]
then
        echo "Second argument is not a file"
        exit 3
fi

f=$1
d=$2

names=$(mktemp)

while read line 
do
        //if ! echo "${line}" | egrep "[A-Za-z-]+ [A-Za-z-]+"
        name=$(echo "${line}" | cut -d ' ' -f 1)
        surname=$(echo "${line}" | cut -d ' ' -f 2)
        echo "${name} ${surname}" >> ${names}        
done < ${f}

cat ${names} | sort | uniq > ${names}

if [ ! -f ${d}/dict.txt ]
then
         touch ${d}/dict.txt         
fi

> ${d}/dict.txt


count=$(cat ${names} | wc -l)

i=1
while read line
do
        echo "${line};${i}" >> ${d}/dict.txt 
        i=$(echo "${i} +1 " | bc)       
done < ${names}
