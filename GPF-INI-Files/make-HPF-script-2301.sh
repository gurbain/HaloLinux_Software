#!/bin/bash
echo "Make HPF Scripts"

CURRDIR=$(pwd)

TIME=$(date -u '+%Y-%m-%d_%H-%M-%S')
HPFLOC="../../HPFs/hpf-$TIME"
HPFDIR="$HPFLOC/halo/"
HPFBUILD="$HPFDIR/build"
HPFBIN="$HPFDIR/build/bin"
HPFPARAM="$HPFDIR/ParameterFiles"
HPFSCRIPTS="$HPFDIR/Scripts"

HPFROOT="../"
HPFROOTBIN="$HPFROOT/build/bin"
HPFROOTSCRIPTS="$HPFROOT/Scripts"
HPFROOTPARAM="$HPFROOT/ParameterFiles"

if [ ! -d $HPFROOTBIN -o ! -d $HPFROOTPARAM -o ! -d $HPFROOTSCRIPTS -o ! -d ../../HPFs ]; then
	exit -1;
fi

echo "Creating directories"

mkdir $HPFLOC
mkdir $HPFDIR
mkdir $HPFBUILD
mkdir $HPFBIN
mkdir $HPFPARAM
mkdir $HPFSCRIPTS

echo "Copying Files"

HPFINI="$HPFROOT/HPF-INI-Files/hpf_2301.ini"

cp $HPFINI $HPFDIR/hpf.ini
cp $HPFINI $HPFLOC/hpf.ini

#############################################

#TP_2301_HaloOpticsMountTest
cp $HPFROOTBIN/TP_2301_HaloOpticsMountTest $HPFBIN
mkdir "$HPFPARAM/TP_2301_HaloOpticsMountTest"
cp $HPFROOTPARAM/TP_2301_HaloOpticsMountTest/pFile-nosphere.txt $HPFPARAM/TP_2301_HaloOpticsMountTest

#Random Scripts
cp $HPFROOTSCRIPTS/chk_network.sh $HPFSCRIPTS
cp $HPFROOTSCRIPTS/restart_ueye.sh $HPFSCRIPTS
cp $HPFROOTSCRIPTS/switch2eth.sh $HPFSCRIPTS

#############################################

echo "Tarring"
cd $HPFDIR
tar -czf halo.hpf ./*
mv halo.hpf ../
cd $CURRDIR
