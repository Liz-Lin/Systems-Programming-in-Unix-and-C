#! /bin/bash
for i in $(who | sort -t 1 | awk '{print $1}' | uniq) 
do
if [[ $(id $i) = *"ugrad"* ]]
then
echo "$i"
fi
done
