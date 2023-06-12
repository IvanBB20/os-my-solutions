#!/ bin / bash

if [ $# -eq 0 ]; then
     exit 1
fi

curDate=$(date +"%Y%m%d")

for i in "$@"; do
     firstLine=$(cat ${i} | head -n 1)

     if ! echo "${firstLine}" | grep -q "SOA"; then
          echo "First line not soa"
          continue
     fi

     if echo "${firstLine}" | egrep -q "([a-z]+\.)+\s+([0-9]+)?\s*IN\s+SOA\s+([a-z0-9]+\.)+\s+([a-z0-9]+\.)+(\s+[0-9]+)+"; then

          #code..

          df=$(echo "${firstLine}" | awk '{print $7}')
          dateFile=$(echo "${df}/100" | bc)
          lastDigits=$(echo "${df}%100" | bc)

          if [ ${dateFile} -eq ${curDate} ]; then
               if [ ${lastDigits} -eq 99 ]; then
                    echo "Last two digits are 99 , cannot go up!"
                    continue
               fi
               lastDigits=$(echo "${lastDigits}+1" | bc)
               if [ ${lastDigits} -ge 10 ]; then
                    sed -i -E "s/${curDate}${lastDigits}/${df}/g" ${i}
               else
                    sed -i -E "s/${curDate}0${lastDigits}/${df}/g" ${i}
               fi
          fi
     elif echo "${firstLine}" | egrep -q "([0-9a-z]+\.)+\s+([0-9]+)?\s*IN\s+SOA\s+([a-z0-9]\.)+\s+([a-z0-9]\.)+\s+\("; then
          #code..
          secondLine=$(cat ${i} | sed -n '2p')
          df=$(echo "${secondLine}" | awk '{print $1}')
          dateFile=$(echo "${df}/100" | bc)
          lastDigits=$(echo "${df}%100" | bc)

          if [ ${dateFile} -eq ${curDate} ]; then
               if [ ${lastDigits} -eq 99 ]; then
                    echo "Last two digits are 99 , cannot go up!"
                    continue
               fi
               lastDigits=$(echo "${lastDigits}+1" | bc)
               if [ ${lastDigits} -ge 10 ]; then
                    sed -i -E "s/${curDate}${lastDigits}/${df}/g" ${i}
               else
                    sed -i -E "s/${curDate}0${lastDigits}/${df}/g" ${i}
               fi
          fi

     fi
done
