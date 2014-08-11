#!/bin/bash
echo VERTIGO MAINTENTANCE
echo "*** Check Network Configuration"
ifconfig
echo
echo "*** Check Wireless Configuration"
iwconfig 2>&1
echo
echo "*** Check Network Interfaces File"
cat /etc/network/interfaces
echo
echo "*** Check IP Route Table"
route
echo VERTIGO MAINTENTANCE COMPLETE
