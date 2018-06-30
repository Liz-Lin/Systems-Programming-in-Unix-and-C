re1='[0-9]'
para=$1
ch1=${para:0:1}
if [[ $ch1 =~ $re1 ]];then
	echo no
elif [[ $para =~ [^a-zA-Z0-9_] ]]; then
	echo no
else
	echo yes
fi
