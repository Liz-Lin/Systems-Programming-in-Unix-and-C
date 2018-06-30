count=0
echo $count : \"$0 \"
for arg in "$@";
do 
	count=$((count+1)) 
	echo $count : \"$arg\" 
done
