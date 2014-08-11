#!/bin/bash
echo VERTIGO MAINTENANCE
echo "*** Switching to Wi-Fi ..."
sleep 1
sudo cp /opt/GogglesDaemon/interfaces.wifi /etc/network/interfaces
echo
echo "*** Restarting Network Service ..."
sudo /etc/init.d/networking restart
echo VERTIGO MAINTENANCE COMPLETE
