#!/bin/bash
LINES="======================================="
STARTMSG="$LINES \nVERTIGO MAINTENANCE SCRIPT"
SUCCESSMSG="SCRIPT SUCCESSFUL"
UNSUCCESSMSG="SCRIPT NOT SUCCESSFUL"
echo -e $STARTMSG
date
GID="$(cat /opt/GogglesDaemon/SIGNATURE)"
echo "Goggles ID: $GID"
echo $0

echo "Empty /home/Results"
sudo mv /home/Results/* /home/TrashResults/Results
ls /home/Results/
ls /home/TrashResults/Results

FILES=/home/TempResults/*
for f in $FILES
do
	FNA='/home/TempResults/Vertigo_Data_2013-02-26_14-00-04_GID_B_00155'
	FNB='/home/TempResults/Vertigo_Data_2013-02-26_14-00-04_GID_B_00235'
	FNC='/home/TempResults/Vertigo_Data_2013-02-26_14-00-04_GID_B_00072'
	FND='/home/TempResults/Vertigo_Data_2013-02-26_14-00-04_GID_B_00040'
	FNE='/home/TempResults/Vertigo_Data_2013-02-26_14-00-04_GID_B_00120'
	if [ "$f" == "$FNA.sdf" -o "$f" == "$FNA-bk.sdf" ] 
	then
		echo "Match A"
		CMD="sudo mv $f $FNA-bk.sdf"
	elif [ "$f" == "$FNB.sdf" -o "$f" == "$FNB-bk.sdf" ] 
	then
		echo "Match B"
		CMD="sudo mv $f $FNB-bk.sdf"
	elif [ "$f" == "$FNC.sdf" -o "$f" == "$FNC-bk.sdf" ] 
	then
		echo "Match C"
		CMD="sudo mv $f $FNC-bk.sdf"
	elif [ "$f" == "$FND.sdf" -o "$f" == "$FND-bk.sdf" ] 
	then
		echo "Match D"
		CMD="sudo mv $f $FND-bk.sdf"
	elif [ "$f" == "$FNE.sdf" -o "$f" == "$FNE-bk.sdf" ] 
	then
		echo "Match E"
		CMD="sudo mv $f $FNE-bk.sdf"
	else
		CMD="sudo mv $f /home/TrashResults/TempResults/"
	fi
	echo $CMD
	$CMD

done

echo "ls /home/Results"
ls /home/Results

echo "ls /home/TempResults"
ls /home/TempResults

echo "ls /home/TrashResults/*"
ls /home/TrashResults/*

echo $SUCCESSMSG


