#!/bin/bash

echo "++Note:"
echo "       ./check.sh \$YourFilePath"

#date="--A1A2B1B2--"
date="--Trace--"
#date="0818test_th80mv"

template=${date}00003.trc
channel=("C1" "C3" "C4")
path=$1
ch1a=$2
ch2a=$3
ch3a=$4
ch1b=" "
ch2b=" "
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
                size=$(stat -c "%s" ${path}/${dirname}/${ch}${template}) 
		echo "The file numbers is : $N"
		echo "The file size is : $size"

			for i in $(seq 0 $(expr $N - 1 ))
			do
			num=$(printf "%05d\n" $i)
			name=${path}/${dirname}/${ch}${date}${num}.trc	
			
                        #echo $name

			if [ ! -e $name ]
			then
			echo $name
			cp ${path}/${dirname}/${ch}${template} $name
                        else
                            filesize=$(stat -c "%s" $name)
                            if [ $filesize -ne $size ]
                            #if [ 0 -eq 1 ]
                            then
			        echo $name
        			cp ${path}/${dirname}/${ch}${template} $name
                            fi

			fi

			done
                        
		 done
	fi
	done
	

else
	for ch in ${channel[@]}
	do	
	N=$[$(ls -l $path/${ch}* | grep "^-" | wc -l)]
        size=$(stat -c "%s" ${path}/${ch}${template}) 
	echo "The file numbers is : $N"
	echo "The file size is : $size"
        
		for i in $(seq 0 $(expr $N - 1 ))
		do
		num=$(printf "%05d\n" $i)
		name=${path}/${ch}${date}${num}.trc	
		#echo $name

		if [ ! -e $name ]
		then
			echo $name
			cp ${path}/${ch}${template} $name
                else
                    filesize=$(stat -c "%s" $name)
                    if [ $filesize -ne $size ]
                    #if [ 0 -eq 1 ]
                    then
		        echo $name
			cp ${path}/${ch}${template} $name
                    fi

		fi

		done
                
	 done
fi	
