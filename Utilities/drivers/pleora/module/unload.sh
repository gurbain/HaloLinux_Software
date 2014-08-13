#!/bin/sh

################################################################################
#
# unload.sh
# Pleora Technologies Inc. Copyright (c) 2002-2014
# for ebUniversalPro-i686.ko 4.1.7 build 3007
#
################################################################################

# Variables
USER_ID=`id -u`

# Display the help for this script
function DisplayHelp()
{
    echo ""
    echo "NAME"
    echo "    unload.sh - Unload the eBUS Universal Pro driver module "
    echo "                ebUniversalPro-i686.ko 4.1.7 build 3007 from the system"
    echo ""
    echo "SYNOPSIS"
    echo "    bash unload.sh [--help]"
    echo ""
    echo "DESCRIPTION"
    echo "    Unload the eBUS Universal Pro module and remove the configure"
	echo "    from the system to be ready to use"
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

# Print out the error and exit 
#  $1 Error message
#  $2 Exit code
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

# Ensure the module is in memory
EBUSUNIVERSALPRO_LOADED=`lsmod | grep -o ebUniversalPro`
if [ "$EBUSUNIVERSALPRO_LOADED" != "ebUniversalPro" ];then
	exit 0
fi

# Unload the module
echo "Unloading eBUS Universal Pro..."
/sbin/rmmod ./ebUniversalPro-i686.ko $* || exit 1

# Remove existing node if any
echo "Delete device node..."
rm -f /dev/ebUniversalPro /dev/ebUniversalPro0
