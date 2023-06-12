#!/bin/bash

if [ $# -ne 1 ]
then
        echo "Wrong number of arguments"
fi

hashAndFile=$(mktemp)
hashes=$(mktemp)

find ${1} -type f -exec sha1sum {} \; | awk '{print $1}' | sort | uniq >> ${hashes}

find ${1} -type f -exec sha1sum {} \; >> ${hashAndFile}

groupCount=0
bytesFreed=0


while read h
do
         count=$(cat ${hashAndFile} | egrep -c "${h}")

         if [ ${count} -eq 1 ]
         then
                continue
         fi 

         groupCount=$(echo "${groupCount} + 1" | bc)

         curFiles=$(mktemp)
         
         candidateFile=$(cat ${hashAndFile} | egrep "${h}" | head -n 1 | awk '{print $2}' )

         cat ${hashAndFile} | egrep "${h}" | awk '{print $2}' | egrep -v "\b${candidateFile}\b" >> ${curFiles}

         while read temp 
         do
                    tempBytes=$(stat "${temp}" --printf "%c\n")
                    rm ${temp}
                    bytesFreed=$(echo "${bytesFreed} + ${tempBytes}" | bc)
                    ln -s "$(realpath ${candidateFile})" "${temp}"
         done < ${curFiles}

        rm ${curFiles}
         
done < ${hashes}

echo "${groupCount} are the number of groups and ${bytesFreed} are the freed bytes"

rm ${hashAndFile}
rm ${hashes}
