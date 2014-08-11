LINES="======================================="
STARTMSG="$LINES \nVERTIGO MAINTENANCE SCRIPT"
SUCCESSMSG="SCRIPT SUCCESSFUL"
UNSUCCESSMSG="SCRIPT NOT SUCCESSFUL"
echo -e $STARTMSG
date
GID="$(cat /opt/GogglesDaemon/SIGNATURE)"
echo "Goggles ID: $GID"
echo $0

sudo mv /home/TrashResults/Results/* /home/Results
sudo mv /home/TrashResults/TempResults/* /home/TempResults
ls /home/Results
ls /home/TempResults
ls /home/TrashResults/*
echo $SUCCESSMSG
