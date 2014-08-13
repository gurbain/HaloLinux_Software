#!/bin/sh

################################################################################
#
# load.sh
# Pleora Technologies Inc. Copyright (c) 2002-2014
# for ebUniversalPro-i686.ko 4.1.7 build 3007
#
################################################################################

# Variables
MACHINE_ARCH=`uname -m`
KERNEL=`uname -r`
USER_ID=`id -u`
VERMAGIC=`modinfo -F vermagic ebUniversalPro-i686.ko`

#Display the help for this script
function DisplayHelp()
{
    echo ""
    echo "NAME"
    echo "    load.sh - Load the eBUS Universal Pro driver module "
    echo "              ebUniversalPro-i686.ko 4.1.7 build 3007"
    echo ""
    echo "SYNOPSIS"
    echo "    bash load.sh [--help]"
    echo ""
    echo "DESCRIPTION"
    echo "    Load the eBUS Universal Pro module and configure the system to"
    echo "    be ready to use"
    echo "    This script can only used by root or sudoer"
    echo "    --help             Display this help"
    echo ""
    echo "COPYRIGHT"
    echo "    Pleora Technologies Inc. Copyright (c) 2002-2014"
    echo ""
    echo "VERSION"
    echo "    4.1.7 build 3007"
    echo ""
}

#Print out the error and exit 
# $1 Error message
# $2 Exit code
function ErrorOut()
{
	echo ""
	echo "Error: $1"
	echo ""
	exit $2
}

# Parse the input arguments
for i in $*
do
    case $i in        
        --help)
            DisplayHelp
            exit 0
        ;;
        *)
        # unknown option
        DisplayHelp
        exit 1
        ;;
    esac
done

# Check required priviledge
if [ "$USER_ID" != "0" ]; then
	ErrorOut "This script can only be run by root user or sudoer" 1
fi

# Do not re-load if not needed
EBUSUNIVERSALPRO_LOADED=`lsmod | grep -o ebUniversalPro`
if [ "$EBUSUNIVERSALPRO_LOADED" == "ebUniversalPro" ];then
	exit 0
fi

# Sanity check
if [ "i686" != "$MACHINE_ARCH" ]; then
    ErrorOut "*** The module ebUniversalPro-i686.ko can only be load with a kernel i686 ***" 1
fi

# Check if this module can be loaded on this kernel
if [ "$KERNEL" != "${VERMAGIC%% *}" ]; then 
    ErrorOut "*** The module ebUniversalPro-i686.ko was not compiled agains this kernel. Use the build.sh script to rebuild it for this kernel ***" 1
fi

# Load the module
echo "Loading eBUS Universal Pro for $MACHINE_ARCH ..."
/sbin/insmod ./ebUniversalPro-i686.ko $* || exit 1

# Remode existing node if any
echo "Delete old device node..."
rm -f /dev/ebUniversalPro0

# Recreate the nodes
echo "Create device node..."
major=$(awk "\$2==\"ebUniversalPro\" {print \$1}" /proc/devices)
mknod /dev/ebUniversalPro c $major 0

# Change permission
echo "Change permission on device node..."
group="staff"
grep -q '^staff:' /etc/group || group="wheel"
chgrp $group /dev/ebUniversalPro
chmod 777 /dev/ebUniversalPro

# Modify the socket configuration
echo "Adjust network settings..."
/sbin/sysctl -w net.core.rmem_max=10485760 > /dev/null
/sbin/sysctl -w net.core.wmem_max=10485760 > /dev/null

