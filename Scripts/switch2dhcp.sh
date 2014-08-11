#!/bin/bash
echo VERTIGO MAINTENANCE
echo "*** Switching Wi-Fi and Ethernet devices to DHCP ..."
sleep 1
sudo cp /opt/GogglesDaemon/interfaces.dhcp /etc/network/interfaces
echo
echo "*** Restarting Network Service ..."
sudo /etc/init.d/networking restart
echo VERTIGO MAINTENANCE COMPLETE
