#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Wrong number of arguments!"
	exit 1
fi

hours=$(echo $1 | bc )

if [ ${hours} -le 1 ]
then
	echo "Arguments is less than 2!"
	exit 1
fi

minutes=$(echo "${hours}*60" | bc)

user=$(whoami)

if [ "${user}" != "oracle" -a "${user}" != "grid" ]
then
	echo "Not oracle or grid!"
	exit 1
fi

if [ -z ${ORACLE_HOME} ]
then
	echo "No ORACLE_HOME variable"
	exit 1
fi


adrci="${ORACLE_HOME}/bin/adrci"
diag_dest="/u01/app/${user}"
if [ ! -x ${adrci} ]
then 
	echo "File not excecutable"
	exit 1
fi

./${adrci}  exec="SET BASE ${diag_dest}"

f=$(mktemp)

if ./${adrci} exec="SET HOMES" | egrep -q "No ADR homes are set"
then
	exit 0
fi	
./${adrci} exec="SET HOMES" >> ${f}

while read path
do
	interesting=$(echo ${path} | cut -d '/' -f 2)
	if [ "${interesting}" == "crs" -o "${interesting}" == "tnslsnr" -o "${interesting}" == "kfod" -o "${interesting}" == "asm" -o "${interesting}" == "rdbms" ]
	then
		./${adrci} exec="SET HOMEPATH ${path}; PURGE ${minutes}"
	fi
done < ${f}
