exec 2>err.txt
users=(`who -q | head -1`)
count=0
user_arr=( $(printf "%s\n"  "${users[@]}" | sort -u) )
for usr in "${user_arr[@]}";
	do
	pattern="${usr} : "	
	usr_g=`groups $usr`
	usr_ng=`echo $usr_g | sed "s/$pattern//g"`
	if [[ $usr_ng =~ ugrad ]];then 
		count=$((count+1))
	fi
done
echo $count

