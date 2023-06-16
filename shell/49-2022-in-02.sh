#!/bin/bash

usr=$(whoami)

if [ "${usr}" != "oracle" -o "${usr}" != "grid" ]
then
	exit 1
fi

if [ $# -ne 1 ]
then
	exit 2
fi

if [  -z ${ORACLE_BASE} ]
then
	exit 1
fi

if [ -z ${ORACLE_HOME} ]
then

	exit 2
fi

if [ -z ${ORACLE_SID} ]
then
	exit 3
fi

role=""

if [ "${usr}" == "oracle" ]
then
	role="SYSDBA"
else
	role="SYSASM"	
fi

t=$(mktemp)
echo "SELECT value FROM v$parameter where name= 'diagnostic_dest';" >> ${t}
echo "EXIT;" >> ${t}

result=$(mktemp)

./${ORACLE_HOME}/bin/sqplus -SL "/ as ${role}" ${t} >> ${result}

if [ $? -ne 0 ]
then
	exit 2
fi


line=$(cat ${result} | sed -n '4p')

diag_base=""


if [ -z "${line}" ]
then
	diag_base=${ORACLE_BASE}
else
	diag_base="diagnostic_dest"
fi

hs=$(hostname -s)
dir="${diag_base}/diag_dir"
crs=$(mktemp)
tnslsnr=$(mktemp)
rdbms=$(mktemp)

if [ "${usr}" == "grid" ]
then
	find ${dir} -mtime +$1 -type f -regextype egrep -regex ".*/crs/${hs}/trace/.*_[0-9]+\.(trc|trm)">> ${crs}
	find ${dir} -mtime +$1 -type f -regextype egrep -regex ".*/tnslsnr/${hs}/.*/alert/.*_[0-9]+\.xml" >> ${tnslnsr}
	find ${dir} -mtime +$1 -type f -regextype egrep -regex ".*/tnslnsr/${hs}/.*/trace/.*_[0-9]+\.log">>${tnslnsr}
else
	find ${dir} -mtime +$1 -mindepth 2 -maxdepth 2 -type f -regextype egrep -regex ".*/rdbms/.*_[0-9]+\.(trc|trm)" >> ${rdbms}
fi

c=0
if [ "${usr}" == "grid" ]
then
	while read line 
	do
	   curB=$(stat ${line} --printf "%s\n")
	   c=$(echo "$c + ${curB}" |bc)
	done < ${crs}
	c=$(echo "$c / 1024" | bc)

	echo "crs : ${c}"
	c=0
	while read line
	do
		cb=$(stat ${line} --printf "%s\n")
		c=$(echo "$c + $cb" | bc)
	done < ${tnslsnr}
	c=$(echo "$c/1024" | bc)
	echo "tnslnsr : ${c}"

else
	while read line
	do
	 	cb=$(stat ${line} --printf "%s")
		c=$(echo "$c + $cb" | bc)
	done < ${rdbms}
	
	c=$(echo "$c/1024" | bc)
	echo "rdbms : $c"	
fi
