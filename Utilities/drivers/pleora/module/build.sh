#!/bin/bash

################################################################################
#
# build.sh
# Pleora Technologies Inc. Copyright (c) 2002-2014
# for ebUniversalPro-i686.ko 4.1.7 build 3007
#
################################################################################

# Variables
CHMOD_BIN=chmod
CP_BIN=cp
ECHO_BIN=echo
MAKE_BIN=make
MKDIR_BIN=mkdir
MV_BIN=mv
POP_BIN=popd
PUSHD_BIN=pushd

KERNEL_VER=`uname -r`
KERNEL_FOLDER="/lib/modules/$KERNEL_VER/build"
MACHINE_ARCH=`uname -m`
OUT_FOLDER=$PWD
SRC_FOLDER=$PWD/ebUniversalProBuilder

#Display the help for this script
function DisplayHelp()
{
    $ECHO_BIN ""
    $ECHO_BIN "NAME"
    $ECHO_BIN "    build.sh - Build the eBUS Universal Pro driver module based"
    $ECHO_BIN "               based on existing transport layer library"
    $ECHO_BIN "               ( libebTransportLayer-i686 )"
    $ECHO_BIN ""
    $ECHO_BIN "SYNOPSIS"
    $ECHO_BIN "    bash build.sh [--output=OUTPUT] [--kernel=KERNEL] [--help]"
    $ECHO_BIN ""
    $ECHO_BIN "DESCRIPTION"
    $ECHO_BIN "    Build the kernel module by linking the localy builded code"
    $ECHO_BIN "    to an existing transport layer library"
    $ECHO_BIN "    --src=SRC_FOLDER   is the location where the source code is"
    $ECHO_BIN "                       locate"
    $ECHO_BIN "    --output=OUTPUT    is the location where the module folder"
    $ECHO_BIN "                       will be create by the script. This folder"
    $ECHO_BIN "                       will contains all theeverything needed for"
    $ECHO_BIN "                       the distribution of the module."
    $ECHO_BIN "    --kernel=KERNEL    point to the kernel header files. This"
    $ECHO_BIN "                       is only optional argument required when"
    $ECHO_BIN "                       cross-compiling for another kernel that"
    $ECHO_BIN "                       is not the one actually in use by your computer"
    $ECHO_BIN "                       i.e. /lib/modules/2.6.18-92.el5/build"
    $ECHO_BIN "    --help             Display this help"
    $ECHO_BIN ""
    $ECHO_BIN "COPYRIGHT"
    $ECHO_BIN "    Pleora Technologies Inc. Copyright (c) 2002-2014"
    $ECHO_BIN ""
    $ECHO_BIN "VERSION"
    $ECHO_BIN "    4.1.7 build 3007"
    $ECHO_BIN ""
}

#Print out the error and exit 
# $1 Error message
# $2 Exit code
function ErrorOut()
{
	$ECHO_BIN ""
	$ECHO_BIN "Error: $1"
	$ECHO_BIN ""
	exit $2
}

# Parse the input arguments
for i in $*
do
    case $i in
        --kernel=*)
            KERNEL_FOLDER=${i#*=} 
            ;;    	
        --src=*)
            SRC_FOLDER=${i#*=} 
            ;;  
        --output=*)
            OUT_FOLDER=${i#*=} 
            ;;    	
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

#Display banner
$ECHO_BIN ""
$ECHO_BIN "*********************************************"
$ECHO_BIN "* Build ebUniversalPro-i686.ko            *"
$ECHO_BIN "*********************************************"
$ECHO_BIN ""

$ECHO_BIN "Use output folder $OUT_FOLDER"
$ECHO_BIN "Use kernel from folder $KERNEL_FOLDER"

#Ensure the kernel header files are availables
if [ ! -d "$KERNEL_FOLDER" ]; then
    ErrorOut "*** Cannot find the files to build kernel module in this PC ***" 1
fi

#Endure we are building on the proper architecture
if [ "i686" != "$MACHINE_ARCH" ]; then
    ErrorOut "*** This package can only be build with a i686 kernel ***" 1
fi

# Now build the module and link the ebTransportLayer
$ECHO_BIN "Call the makefile to build the ebUniversalPro-i686.ko module"
$MAKE_BIN -C"$SRC_FOLDER" "KERNEL_FOLDER=$KERNEL_FOLDER" "all"
if [ ! -f "$SRC_FOLDER/ebUniversalPro-i686.ko" ]; then
	ErrorOut "*** Fail to create the module ebUniversalPro-i686.ko ***" 1
fi

if [ "$SRC_FOLDER" != "$OUT_FOLDER" ]; then
    # Create the output directory
    $ECHO_BIN "Create output directory $OUT_FOLDER"
    $MKDIR_BIN "-p" "$OUT_FOLDER"

    # Populate the output folder 
    $ECHO_BIN "Move the output files to $OUT_FOLDER for distribution"
    $MV_BIN "$SRC_FOLDER/ebUniversalPro-i686.ko" "$OUT_FOLDER"
    $CHMOD_BIN a+x "$OUT_FOLDER/ebUniversalPro-i686.ko"
fi

# Cleanup the module builder folder to be ready for distribution
$MAKE_BIN -C"$SRC_FOLDER" "KERNEL_DIR=$KERNEL_FOLDER" "clean" 

$ECHO_BIN ""
$ECHO_BIN "The module ebUniversalPro-i686.ko is now available."
$ECHO_BIN "Use the load.sh script to load it in memory"
$ECHO_BIN ""
$ECHO_BIN "Do not forget to rebuild the module after each kernel update!"
$ECHO_BIN ""

