#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Wrong number of args"
fi

dir=$1

if [ ! -d ${dir} ]; then
    echo "Not a directory"
fi

friends=$(mktemp)
find ${dir} -mindepth 4 -maxdepth 4 -type d | sort | uniq >> ${friends}

global=$(mktemp)

while read fr
do
        count=$(find ${dir} -type f -regextype egrep -regex ".*/${fr}/[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}\.txt" -exec wc -l {} \; | awk 'BEGIN {s=0} {s+=$1}  END {print s}')
        echo "${count} ${fr}" >> ${global}
done < ${friends}


cat ${global} | sort -k1 -nr | head -n 10

rm ${global}
rm ${friends}
