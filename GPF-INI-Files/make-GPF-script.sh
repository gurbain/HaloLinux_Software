#!/bin/bash
echo "Make GPF Scripts"

CURRDIR=$(pwd)

TIME=$(date -u '+%Y-%m-%d_%H-%M-%S')
GPFLOC="../../GPFs/gpf-$TIME"
GPFDIR="$GPFLOC/vertigo/"
GPFBUILD="$GPFDIR/build"
GPFBIN="$GPFDIR/build/bin"
GPFPARAM="$GPFDIR/ParameterFiles"
GPFSCRIPTS="$GPFDIR/Scripts"

GPFROOT="../"
GPFROOTBIN="$GPFROOT/build/bin"
GPFROOTSCRIPTS="$GPFROOT/Scripts"
GPFROOTPARAM="$GPFROOT/ParameterFiles"

if [ ! -d $GPFROOTBIN -o ! -d $GPFROOTPARAM -o ! -d $GPFROOTSCRIPTS -o ! -d ../../GPFs ]; then
	exit -1;
fi

echo "Creating directories"

mkdir $GPFLOC
mkdir $GPFDIR
mkdir $GPFBUILD
mkdir $GPFBIN
mkdir $GPFPARAM
mkdir $GPFSCRIPTS

echo "Copying Files"

GPFINI="$GPFROOT/GPF-INI-Files/gpf_4000.ini"

cp $GPFINI $GPFDIR/gpf.ini
cp $GPFINI $GPFLOC/gpf.ini

#############################################

#TP_2301_HaloOpticsMountTest
cp $GPFROOTBIN/TP_2301_HaloOpticsMountTest $GPFBIN
cp $GPFROOTSCRIPTS/camera_calib.sh $GPFSCRIPTS
mkdir "$GPFPARAM/TP_2301_HaloOpticsMountTest"
cp $GPFROOTPARAM/TP_2301_HaloOpticsMountTest/pFile-nosphere.txt $GPFPARAM/TP_2301_HaloOpticsMountTest


#Random Scripts
cp $GPFROOTSCRIPTS/chk_network.sh $GPFSCRIPTS
cp $GPFROOTSCRIPTS/restart_ueye.sh $GPFSCRIPTS
cp $GPFROOTSCRIPTS/switch2eth.sh $GPFSCRIPTS

#############################################

echo "Tarring"
cd $GPFDIR
tar -czf vertigo.gpf ./*
mv vertigo.gpf ../
cd $CURRDIR
