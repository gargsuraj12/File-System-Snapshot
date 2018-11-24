#!/bin/bash

cd $1

ls -l| grep "^[^t]" |

awk '/^[-]/ {print $NF}' | cat > temp.txt

file=(`cat temp.txt`)

for t in "${file[@]}"
do
	extension="${t#*.}"
	for w in "$@"
	do
		if test $w == $extension
		then
			echo "$extension $t"
			mkdir -p $extension
			mv $t $extension
			break
		fi			
	done	
done




