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
GPFRESULTS="$GPFDIR/Results"

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
mkdir $GPFRESULTS

echo "Copying Files"

GPFINI="$GPFROOT/GPF-INI-Files/gpf_3370.ini"

cp $GPFINI $GPFDIR/gpf.ini
cp $GPFINI $GPFLOC/gpf.ini

#############################################

#TP_1020_Checkout
cp $GPFROOTBIN/TP_1020_Checkout $GPFBIN
cp $GPFROOTSCRIPTS/camera_calib.sh $GPFSCRIPTS
mkdir "$GPFPARAM/TP_1020_Checkout"
cp $GPFROOTPARAM/TP_1020_Checkout/pFile_calib_v1.txt $GPFPARAM/TP_1020_Checkout

#TP_1031_BlobTrack
cp $GPFROOTBIN/TP_1031_BlobTrack $GPFBIN
mkdir "$GPFPARAM/TP_1031_BlobTrack"
cp $GPFROOTPARAM/TP_1031_BlobTrack/pFile_3370.txt $GPFPARAM/TP_1031_BlobTrack

#Random Scripts
cp $GPFROOTSCRIPTS/setup_results_directories.sh $GPFSCRIPTS
cp $GPFROOTSCRIPTS/chk_network.sh $GPFSCRIPTS
cp $GPFROOTSCRIPTS/switch2eth.sh $GPFSCRIPTS


#############################################

echo "Tarring"
cd $GPFDIR
tar -czf vertigo.gpf ./*
mv vertigo.gpf ../
cd $CURRDIR
