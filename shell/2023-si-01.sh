#!/bin/bash

if [ $# -ne 2 ]
then
        exit 1
fi

file=$1
dir=$2

if [ ! -f ${file} ]
then
        exit 2
fi

if [ ! -d ${dir} ]
then
        exit 3
fi

paths=$(mktemp)

find ${dir} -type f -regextype egrep -regex ".*/.*\.txt" >> ${paths}

for i  in $(cat ${paths})
do
        while read line 
        do
                if ! cat ${i} | egrep -q "${line}"
                then
                        continue
                fi
                len=$(echo -n "${line}" | wc -c)
                censored=""
                star="*"
                for j in $(seq 1 ${len})
                do
                        censored=${censored}${star}
                done
                sed -i -E "s/\b${line}\b/${censored}/g" ${i}
        done < ${file}
done

