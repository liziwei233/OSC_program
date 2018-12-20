#!/bin/bash

echo "++Note:"
echo "       ./check.sh \$YourFilePath"

date="trace"
template=${date}00001.trc
channel=("C1" "C3" "C4")
path=$1
ch1a=$2
ch2a=$3
ch3a=$4
ch1b=C3
ch2b=C4
ch3b=" "
ch1=${ch1a:-$ch1b}
ch2=${ch2a:-$ch2b}
ch3=${ch3a:-$ch3b}
channel=($ch1 $ch2 $ch3)

subdir_num=$(ls -l $path | grep "^d" | wc -l)
flag=0
if [ ${path:0-1} == "/" ]
then 
path=${path%/*}
fi

if [ $subdir_num -gt $flag ]
then
	#echo $subdir_num
	for dirname in `ls $path`
	do
	echo "Open the direct : $dirname"
	if [ -d $path/$dirname ]
	then
		for ch in ${channel[@]}
		do	
		N=$[$(ls -l $path/$dirname/${ch}* | grep "^-" | wc -l)]
		echo "The file numbers is : $N"
	
			for i in $(seq 0 $(expr $N - 1 ))
			do
			num=$(printf "%05d\n" $i)
			name=${path}/${dirname}/${ch}${date}${num}.trc	
			#echo $name

			if [ ! -e $name ]
			then
			echo $name
			cp ${path}/${dirname}/${ch}${template} $name
			fi

			done	
		 done
	fi
	done
	

else
	for ch in ${channel[@]}
	do	
	N=$[$(ls -l $path/${ch}* | grep "^-" | wc -l)]
	echo "The file numbers is : $N"
	
		for i in $(seq 0 $(expr $N - 1 ))
		do
		num=$(printf "%05d\n" $i)
		name=${path}/${ch}${date}${num}.trc	
		#echo $name

		if [ ! -e $name ]
		then
			echo $name
			cp ${path}/${ch}${template} $name
		fi

		done
	 done
fi	