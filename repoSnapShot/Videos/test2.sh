#!/bin/bash

cd $1

ls -l| grep "^[^t]" |

awk '/^[-]/ {print $NF}' | cat > temp.txt

file=(`cat temp.txt`)

ext="all"

if test $2 == $ext
then
	echo "here"
for t in "${file[@]}"
do
	extension="${t#*.}"
	for w in "$@"
	do
		mkdir -p $extension
		mv $t $extension
		break
	done	
done

else	
for t in "${file[@]}"
do
	extension="${t#*.}"
	for w in "$@"
	do
		if test $w == $extension
		then
			mkdir -p $extension
			mv $t $extension
			break
		fi			
	done	
done
fi



