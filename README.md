
HALO SOFTWARE FOR LINUX UBUNTU 10.04
====================================

This version integrates the core for ORF and Thermocam into the Halo core to run on Linux Ubuntu 10.04 with a themocam FLIR A5, an optics mount with two ueye stereo cameras and a Optical Range Finder MESA SR4000.


Download:
---------

To download the Halo core on a Linux computer, type in a shell:
```
cd ~
git clone https://github.com/Gabs48/HaloLinux_Software
```

Compilation:
------------

To compile the core, create a build directory, config with cmake and compile with make using make:
```
cd HaloLinux_Software
mkdir build
cd build
cmake ..
make
```

Installation:
-------------

To install the software, create a script and add its link in the linux path so that you can start it from everywere in a shell. Begin with:
```
cd ~
mkdir bin
cd bin
```
Then create the script all in verifying your pleora version (for example it could be *.../ebus_sdk/RHEL-6-x86_64/...*). On VERTIGO computer, the version used is *RHEL-6-i686*:
```
echo source /opt/pleora/ebus_sdk/RHEL-6-i686/bin/set_puregev_env >> test_Halo
echo cd ~/HaloLinux_Software/build/bin/ >> test_Halo
echo ./TP_2601_SaveOpticsOrfThermo ~/HaloLinux_Software pFile-nosphere.txt >> test_Halo
chmod +x test_Halo
```
Finally, source the script in your .bashrc which wil be sourced itself everytime you start a shell session:
```
echo export PATH=$PATH:~/bin >> ~/.bashrc
source ~/.bashrc
```

Configuration:
-------------

You can change the parameters of the software in ~/HaloLinux_Software/ParameterFiles/TP_2601_SaveOpticsOrfThermo/pFile-nosphere.txt:
- USE_THERMOCAM: to take ad save pictures from the thermocam
- USE_ORF: to take and save pictures from the orf
- USE_OTICSMOUNT: to take and save pictures from the opticsmount
- ORF_AUTO_EXPOSURE: set auto_exposuer for the ORF (advice: do not change)
- ORF_AMP_TRESH: set the amplitude threshold for ORF images (advice: do not change)
- ORF_MOD_FREQ: set the modulation frequency of the ORF emitter/receiver
- ORF_INTEGRATION_TIME: set the time of integration for denoising the ORF (advice: do not change)
- ORF_IP_ADDR: set the IP of he ORF device (by default with no dhcp server, it takes the value 192.168.1.42)
- ORF_CALIB_FILENAME: the path and the filename for the ORF caibration file. If it's not found, a calibration wll be launched by default (requires a GUI and a checkerboard)
- ORF_SAVE_DIR: the directory where the pictures will be saved. **WARNING**: Verify the directory is created before starting the program to avoid any error
- AUTOMATIC_IMAGE_STORAGE_OM: this parameter from the halo core should be set to false as the images are already saved in the test project

Execution:
-----------

Finally, assemble Halo. Disconnect the ethernet cable from ORF camera. Power on Halo and wait a dozen of seconds before plugging the ethernet cable back in the ORF. Verify the ORF IP address is well set and start the program:
```
ping 192.168.1.42
test_Halo
```

Known issues:
-------------

During compilation:
- The defines *WORD* and *DWORD* in /usr/include/definesSR.h and usr/include/libMesaSR.h are already defined and need to have another name
- The CMakeLists use a wrong path for pleora libraries and includes (see CMakesLists in testproject and ThermoCamCore

During execution:
- The pleora env are not sourced
- The IP is badly configured. The thermocam IP must be set manually to 192.168.1.41, the computer IP must be set in manual mode to 192.168.1.1, the ORF takes automatically 192.168.1.42
- The OpticsMount IDs in /opt/GogglesOptics/CAMERA_FILE are wrong
- The Calibration parameters *extrinsics.yml*  *intrinsics.yml*  *ORF_calib.xml* must be placed by default in /opt/GogglesOptics/Calib_Params/defaultSet/
- The parameters are set up badly

