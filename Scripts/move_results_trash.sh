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

sudo mv /home/Results/* /home/TrashResults/Results
sudo mv /home/TempResults/* /home/TrashResults/TempResults
echo $SUCCESSMSG
