#!/bin/bash
echo VERTIGO MAINTENTANCE
echo "*** Camera Calibration Procedure"
echo Please switch to Video View panel.
echo To stop this script, switch to Video View panel
echo and press the ESCAPE KEY to STOP the Camera
echo Calibration Procedure.
echo
sleep 1

/home/GPF_DIR/build/bin/TP_1020_Checkout /home/GPF_DIR CURRENT_CALIB_DIR pFile_calib_v1_brighter.txt

echo
echo VERTIGO MAINTENTANCE COMPLETE
