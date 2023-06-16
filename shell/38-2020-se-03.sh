#!/bin/bash

if [ $# -ne 2 ]
then
        exit 1
fi

repo=$1
packed=$2

if [ ! -d ${repo} ]
then
        exit 2
fi

if [ ! -d ${packed} ]
then
        exit 3
fi

tar -cJvf a.tar.xz ${packed}/tree

sum=$(sha256sum a.tar.xz | cut -d ' ' -f 1)
bsName=$(basename ${packed})
version=$(cat ${packed}/version | head -n 1)

if ! cat ${repo}/db | egrep -q "${bsName}-${version}"
then
        echo "${bsName}-${version} ${sum}" >> ${repo}/db
        sort ${repo}/db > ${repo}/db
        tar -cJvf ${repo}/"${sum}.tar.xz" ${packed}/tree
        exit 0
fi

if cat ${repo}/db | egrep -q "${bsName}-${version} ${sum}"
then
        exit 0
fi
oldSum=$(cat ${repo}/db | egrep "${bsName}-${version}" | cut -d ' ' -f 2)


sed -i -E "s/${bsName}-${version} ${oldSum}/${bsName}-${version} ${sum}/" ${repo}/db

sort ${repo}/db > ${repo}/db

rm -r ${repo}/${oldSum}.tar.xz
tar -cJvf ${repo}/${sum}.tar.xz ${packed}/tree

###################################### or
#!/bin/bash

if [[ $# -ne 2 ]]; then
	echo "Expecting 2 arguments"
	exit 1
fi


if [[ ! -d $1 ]]; then
	echo "Expecting directory"
	exit 2
fi

if [[ ! -d $2 ]]; then
	echo "Expecting directory"
    exit 2
fi

pkg=$2
repo=$1

temp_repo=$(mktemp)
tar -cvJf $temp_repo $pkg/tree

pkg_version="$(basename $pkg)-$(cat $pkg/version)"
sum=$(sha256sum $temp_repo | cut -d ' ' -f1 )

mv -f "$temp_repo" "$repo/packages/$sum.tar.xz"  

temp_db=$(mktemp)
cat $repo/db | awk -v v=$pkg_version -v sum=$sum '$1 != v {print v, sum}' > $temp_db  

sort -v $temp_db >> cat $repo/db 

rm $temp_db $temp_repo
