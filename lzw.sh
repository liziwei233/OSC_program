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
echo "		./lzw.sh \$YourFilePath \$Yourchannel"
path=$1
pathname=$(basename $path)
pre=''
flag=0
channel=$2

subdir_num=$(ls -l $path | grep "^d" | wc -l)
if [ $subdir_num -gt $flag ]
then

	for name in `ls $path`
	do

		echo $name
		if [ -d $path/$name ]
		then
			Num=$(ls -l $path/$name | grep "^d" | wc -l)
			nfile=$(ls -l $path/$name/${channel}* | grep "^-" | wc -l)
			#nfile=$(expr $(ls -l $path/$name/${channel}* | grep "^-" | wc -l) - 1)
			echo $Num
			echo $nfile

			if [ $Num -eq $flag ]
			then
				echo $pre
				echo "Nfile =" $((nfile-1))
				./analyze_testbeam $path/$name $((nfile-1)) $path/$pathname$name.root
			else
				for subname in `ls $path/$name`
				do
					nsubfile=$(ls -l $path/$name/$subname | grep "^-" | wc -l)
					echo "Nfile =" $((nsubfile-1))
					./analyze_testbeam $path/$name/$subname $((nsubfile-1)) $name_$subname.root
				done
			fi
		fi

	done
else

	nfile=$(ls -l $path/${channel}* | grep "^-" | wc -l)
	#nfile=$(expr $(ls -l $path/${channel}* | grep "^-" | wc -l) - 1)
	echo "Nfile =" $((nfile-1))
	echo $pre
	./analyze_testbeam $path/ $((nfile-1)) $path/../${pathname}.root
fi

