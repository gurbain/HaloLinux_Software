Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.

========================
Qt SDK
========================

The eBUS SDK is build using the Qt SDK 4.6.2 distributed with 
Red Hat Enterprise Linux Workstation release 6.1 (Santiago).

The sample Makefile is assuming that the development package is installed and that the
qmake location is in the system PATH.

========================
FFmpeg
========================

Some eBUS SDK samples are using the ffmpeg to manipulated images and has an example 
of 3rd party SDK. 

To compile these sample, the user will have to install development package for ffmpeg 0.11.X (or adapt 
the sample to any other versions). 

ffmpeg 0.11.X can be downloaded from:
    http://www.ffmpeg.org/download.html
Uncompress the package in the folder of your choice.

In a terminal, type the following commands:
    cd [ folder ] (folder is the location of the uncompressed ffmpeg package containing source code, INSTALL, CMakeLists.txt etc.).
    ./configure --enable-shared --disable-yasm
    make
	
To complete the installation, run the following command as root (or sudoer).
    make install
All the default locations should be preserved to allows the Makefiles to find the package components.

The validation was done using FFmpeg 0.11.2.

========================
Cross-compiling the sample
========================
Before cross-compiling the sample, the user must ensure that all the required environment for 
its toolschain is configure properly. It must also define the environment variables CC, CPP and LD
to point to you gcc, g++ and ld version of the toolschains.
The pleora/ebus_sdk/ARCH/bin/set_puregev_env.sh must be sourced to define the configuration / location of the SDK as well.



