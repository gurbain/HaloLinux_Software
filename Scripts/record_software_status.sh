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

echo $LINES
echo "Checking Process Information"
SYSCALL="top -b n1 H"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

echo $LINES
echo "Checking Network Configuration"
SYSCALL="ifconfig"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

SYSCALL="iwconfig"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL 2>&1

SYSCALL="route"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

echo $LINES
echo "Checking USB and Device Configuration"
SYSCALL="lsusb"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

SYSCALL="lsmod"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

echo $LINES
echo "Checking Processor Information"
SYSCALL="cat /proc/cpuinfo"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

SYSCALL="cat /proc/meminfo"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL


echo $LINES
echo "Checking Disk Information"
SYSCALL="df -h"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL

echo $LINES
echo "Checking Package Information"
SYSCALL="dpkg --get-selections"
echo -e "SYSTEM CALL: $SYSCALL"
$SYSCALL


echo $LINES
echo -e $SUCCESSMSG
