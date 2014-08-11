#!/bin/bash
echo VERTIGO MAINTENTANCE
echo "*** Check USB Devices (lsusb)"
sudo lsusb
echo
echo "*** Check Modules (lsmod)"
lsmod
echo VERTIGO MAINTENTANCE COMPLETE
