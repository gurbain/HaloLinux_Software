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

echo "Making New Directories"
echo $LINES
RESULTS_DIR="/home/Results"
sudo mkdir $RESULTS_DIR
sudo chmod 777 $RESULTS_DIR

TEMP_RESULTS_DIR="/home/TempResults"
sudo mkdir $TEMP_RESULTS_DIR
sudo chmod 777 $TEMP_RESULTS_DIR

TEMP_RESULTS_DIR2="/home2/TempResults"
sudo mkdir $TEMP_RESULTS_DIR2
sudo chmod 777 $TEMP_RESULTS_DIR2

TRASH="/home/TrashResults"
TRASH_TEMP="/home/TrashResults/TempResults"
TRASH_RESULTS="/home/TrashResults/Results"
sudo mkdir $TRASH
sudo mkdir $TRASH_RESULTS
sudo mkdir $TRASH_TEMP
sudo chmod -R 777 $TRASH

echo $LINES
echo "Changes Completed - Checking New Directory Info"
SYSCALL="ls -al /home"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

SYSCALL="ls -al /home2"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

SYSCALL="ls -al /home/TrashResults"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL


echo $LINES
if [ ! -d "/home/Results" ] ; then
	echo "/home/Results Missing!"
fi
if [ ! -d "/home/TempResults" ] ; then
	echo "/home/TempResults Missing!"
fi
if [ ! -d "/home2/TempResults" ] ; then
	echo "/home2/TempResults Missing!"
fi
if [ ! -d "/home/TrashResults" ] ; then
	echo "/home/TrashResults Missing!"
fi
if [ ! -d "/home/TrashResults/Results" ] ; then
	echo "/home/TrashResults/Results Missing!"
fi
if [ ! -d "/home/TrashResults/TempResults" ] ; then
	echo "/home/TrashResults/TempResults Missing!"
fi
if [ -d "/home/Results" -a -d "/home/TempResults" -a -d "/home2/TempResults" -a -d "/home/TrashResults" -a -d "/home/TrashResults/Results" -a -d "/home/TrashResults/TempResults" ] ; then
	echo $SUCCESSMSG
else
	echo $UNSUCCESSMSG
fi
