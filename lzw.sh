#!/bin/bash
#NAME=/baby/one/more/time
#echo $NAME>name.log
#FULLNAME=$(cat name.log)
#echo $FULLNAME
#POS=$(dirname $FULLNAME)
#echo $POS
#NAME=$(basename $POS)
#echo $NAME

echo "++Note:"
echo "		./lzw.sh [YourFilePath] [id] [channel]"
path=$1
ID=$2
pathname=$(basename $path)
pre=''
flag=0
channel=$3
echo "The number of waveforms you chose is : $ID " 
subdir_num=$(ls -l $path | grep "^d" | wc -l)
if [ $subdir_num -gt $flag ]
then

	for name in `ls $path`
	do

		echo "subfile's name in $path is $name"
		if [ -d $path/$name ]
		then
			Num=$(ls -l $path/$name | grep "^d" | wc -l)
			nfile=$(expr $(ls -l $path/$name/${channel}* | grep "^-" | wc -l) - 1)
			echo "the numbers of directory is $Num"
			echo "$path/$name/${channel} have $nfile files"

			if [ $Num -eq $flag ]
			then
				echo "the prefix name is $pre"
				#./analyze_testbeam $path/$name $nfile ../$pathname$name.root $ID
				./analyze_testbeam $path/$name 1 ../$pathname$name.root $ID
			else
				for subname in `ls $path/$name`
				do
					nsubfile=$(ls -l $path/$name/$subname | grep "^-" | wc -l)
					#./analyze_testbeam $path/$name/$subname $nsubfile ../$name_$subname.root $ID
					./analyze_testbeam $path/$name/$subname 1 ../$name_$subname.root $ID
				done
			fi
		fi

	done
else

	nfile=$(ls -l $path/${channel}* | grep "^-" | wc -l)
	#nfile=$(expr $(ls -l $path/${channel}* | grep "^-" | wc -l) - 1)
	echo "$path/${channel} have $nfile files"
	echo "the prefix name is $pre "
        echo " you enter here"
	#./analyze_testbeam $path/ $nfile ../${pathname}.root $ID
	./analyze_testbeam $path/ 1 ../${pathname}.root $ID
	echo "./analyze_testbeam $path/ $nfile ../${pathname}.root $ID"

fi

