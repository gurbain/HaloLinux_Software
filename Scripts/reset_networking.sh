LINES="======================================="
STARTMSG="$LINES \nVERTIGO MAINTENANCE SCRIPT"
SUCCESSMSG="SCRIPT SUCCESSFUL"
UNSUCCESSMSG="SCRIPT NOT SUCCESSFUL"

IPADDR="10.11.0.46"

echo -e $STARTMSG
date
GID="$(cat /opt/GogglesDaemon/SIGNATURE)"
echo "Goggles ID: $GID"
echo $0
sudo cp /opt/GogglesDaemon/interfaces.eth /etc/network/interfaces
echo
echo "*** Restarting Network Service ..."
sudo /etc/init.d/networking restart

ping -c 5 -w 20 $IPADDR
RETVAL=$?
echo $LINES
[ $RETVAL -eq 0 ] && echo $SUCCESSMSG
[ $RETVAL -ne 0 ] && echo $UNSUCCESSMSG

