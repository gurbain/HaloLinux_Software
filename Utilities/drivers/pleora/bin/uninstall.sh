#!/bin/sh

# Default variables
USR_LIB=/usr/lib

START_DIR=`dirname $0`
START_DIR=`cd $START_DIR/..; pwd`

INSTALL_ROOT=$START_DIR

REMOVE_LINKS=yes
INSTALL_DIR_OVERWRITE=yes

ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`

# Create the set_puregev_vars script
if [ "i686" = "x86_64" ]; then
  GENICAM_LIB_SUBDIR=Linux64_x64
else
    if [ "i686" = "i686" ]; then
        GENICAM_LIB_SUBDIR=Linux32_i86
    else
      if [ "i686" = "arm" ]; then 
        GENICAM_LIB_SUBDIR=Linux32_ARM
      fi
    fi
fi

# Initial screen
clear
echo "eBUS_SDK 3.1.7 for Linux"
echo "  ( RHEL-6-i686 )"
echo "========================================"
echo ""

echo -n "Remove eBUS_SDK installed in directory '$INSTALL_ROOT' (yes/no) ? "
read ANSWER
if [ ! "$ANSWER" = "yes" ]; then
  echo "Uninstall cancelled."
  exit;
fi

if [ ! -w $INSTALL_ROOT ]; then
  echo "You do not have write permissions to '$INSTALL_ROOT'."
  echo "Run installer as super user (root)."
  exit 1
fi

if [ "$ARCH" = "i686" ]; then
  # We only remove the driver is we are a native SDK for this PC
  EBUSUNIVERSALPRO_LOADED=`lsmod | grep -o ebUniversalPro`
  if [ "$EBUSUNIVERSALPRO_LOADED" == "ebUniversalPro" ];then
    if [ -e "$INSTALL_ROOT/module/unload.sh" ]; then
      bash "$INSTALL_ROOT/module/unload.sh"
    fi
  fi
fi

if [ "$ARCH" = "i686" ]; then
  # We only remove the symbolic link if we are native to this SDK
  if [ "$REMOVE_LINKS" = "yes" ]; then
    rm -rf /etc/ld.so.conf.d/eBUS_SDK.conf
    /sbin/ldconfig
  fi
fi

echo "Removing software on $INSTALL_ROOT"
if [ -d $INSTALL_ROOT ]; then
  EXCEPTION_FOLDER=""
  if [ -d "$INSTALL_ROOT/licenses" ]; then
    if [ -n "$( ls $INSTALL_ROOT/licenses/*.lic 2> /dev/null )" ]; then
        EXCEPTION_FOLDER="licenses"
    fi
  fi
  if [ -z "$EXCEPTION_FOLDER" ]; then
    rm -rf $INSTALL_ROOT
    rmdir $(readlink -m $INSTALL_ROOT/..) &> /dev/null
    rmdir $(readlink -m $INSTALL_ROOT/../..) &> /dev/null
    rmdir $(readlink -m $INSTALL_ROOT/../../..) &> /dev/null
  else
    ls -d $INSTALL_ROOT/* -1 | grep -v -E $EXCEPTION_FOLDER | xargs rm -rf
  fi
fi

echo "Removal complete."
echo ""
echo ""
