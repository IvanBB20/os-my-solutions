#!/bin/bash


match=$(mktemp)

while read line
do
	usr=$(echo "${line}" | cut -d ':' -f 1)
	directory=$(echo "${line}" | cut -d ':' -f 6)
	if [ !  -d "${directory}" ]
	then
		echo "${usr}" >> ${match}
		continue
	fi

	dirOwner=$(stat "${directory}" --printf "%U\n")

	if [ "${dirOwner}" != "${usr}" ]
	then
		echo "${usr}" >> ${match}
		continue
	fi
	#drwxrwxrwx
	perm=$(stat "${directory}" --printf "%A\n" | cut -c 3)
	if [ "${perm}" != "w" ]
	then
		echo "${usr}" >> ${match}
		continue
	fi
done < /etc/passwd


sumRoot=$(ps -u root  -o rss | awk 'BEGIN {sum=0} {sum+=$1} END {print sum}')
echo "Sum root is : ${sumRoot}"

while read user
do
		sumCurUser=$(ps -u ${user} -o rss | awk 'BEGIN {s=0} {s+=$1} END {print s}')
		if [ ${sumCurUser} -gt ${sumRoot} ]
		then		
				echo "I will kill pids"
				ps -u "${user}" -o pid --no-headers | xargs kill 
		fi	
		#echo "${sumCurUser} and ${sumRoot}"
done < ${match}

rm ${match}
