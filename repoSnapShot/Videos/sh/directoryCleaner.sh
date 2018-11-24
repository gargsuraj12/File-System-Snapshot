#!/bini/bash

ls -l| grep "^[^t]" |

awk '/^[-]/ {print $NF}' | cat > temp.txt

file=(`cat temp.txt`)

for t in "${file[@]}"
do
	extension="${t#*.}"

	echo "$extension $t"

		mkdir -p $extension
		mv $t $extension		
		
done




