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

#TP_2302_HaloOpticsMountTest_rev2
cp $GPFROOTBIN/TP_2302_HaloOpticsMountTest_rev2 $GPFBIN
mkdir "$GPFPARAM/TP_2302_HaloOpticsMountTest_rev2"
cp $GPFROOTPARAM/TP_2302_HaloOpticsMountTest_rev2/pFile-nosphere.txt $GPFPARAM/TP_2302_HaloOpticsMountTest_rev2

#TP_2303_HaloDockingPortTest
cp $GPFROOTBIN/TP_2303_HaloDockingPortTest $GPFBIN
mkdir "$GPFPARAM/TP_2303_HaloDockingPortTest"
cp $GPFROOTPARAM/TP_2303_HaloDockingPortTest/pFile-nosphere.txt $GPFPARAM/TP_2303_HaloDockingPortTest

#TP_2304_HaloOpticsAndXSCamTest
cp $GPFROOTBIN/TP_2304_HaloOpticsAndXSCamTest $GPFBIN
mkdir "$GPFPARAM/TP_2304_HaloOpticsAndXSCamTest"
cp $GPFROOTPARAM/TP_2304_HaloOpticsAndXSCamTest/pFile-nosphere.txt $GPFPARAM/TP_2304_HaloOpticsAndXSCamTest

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
