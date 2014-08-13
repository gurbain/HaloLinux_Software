
HALO SOFTWARE FOR LINUX UBUNTU 10.04
====================================

This version integrates the core for ORF and Thermocam into the Halo core to run on Linux Ubuntu 10.04 with a themocam FLIR A5, an optics mount with two ueye stereo cameras and a Optical Range Finder MESA SR4000.


1. Download:
------------

To download the Halo core on a Linux computer, type in a shell:
```
cd ~
git clone https://github.com/Gabs48/HaloLinux_Software
```

2. Install Drivers:
-------------------
If it is the first time you are installing the software on your computer, you may need to install the ORF and the thermocam deriver first! If not, you can skip this step.

**Install MESA-IMAGING 3D CAM Drivers**
- Go to http://www.mesa-imaging.ch/support/driver-downloads/
- Download the linux driver you need (VERTIGO computer runs in 32bits)
- Double-click on the .deb file you intalled and follow the instructions
- **WARNING: ** There is a conflict in the *defines* provided from MESA-IMAGING and OpenCV. To solve that conflict, simply replace the files *defineSR.h* and *libMesaSR.h* by new one provided in the folder ~/HaloLinux_Software/Utilities/drivers/mesa:
```
cd ~/HaloLinux_Software/Utilities/drivers/mesa
sudo mv libMesaSR.h defineSR.h /usr/include
```

**Install PLEORA Thermocam Drivers**
- If you are running a 32bits version of Ubuntu 10.04 with a x86 processor architecture, copy ~/HaloLinux_Software/Utilities/drivers/pleora into the right directory:
```
cd ~/HaloLinux_Software/Utilities/drivers/pleora
sudo mkdir /opt/pleora/ebus_sdk/RHEL-6-i686/
sudo mv -r * /opt/pleora/ebus_sdk/RHEL-6-i686/
```


3. Compile:
-----------

To compile the core, create a build directory, config with cmake and compile with make using make:
```
cd ~/HaloLinux_Software
mkdir build
cd build
cmake ..
make
```

4. Install Software:
--------------------

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

5. Configure Software:
---------------------

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


7. Configure IP addresses
-------------------------

To facilitate the ethernet communication between each device, we  will give every devices a fix IP address on the network 192.168.1.* with a submask 255.255.255.0. If you already performed this step previously, you can skip it. If it is the first time, we need to fix the address for every devices:

- **Vertigo computer**: In the file /etc/network/interfaces verify those lines are added:
```
auto eth0
iface eth0 inet static
address 192.168.1.1
netmask 255.255.255.0
```
Then, you can restar the network with the command:
```
sudo /etc/init.d/networking restart
```

- **ORF sensor**: By default, the IP address it takes automatically the value 192.168.1.42. If you want to change it, you have to open a telnet communication with it (being on the network 192.168.1.*) and then change it. However, it doesn't seem to work, so we'll keep the automatic address to avoid any problem.

- **Thermocam sensor**: The best way to find or change the thermocam adress is to execute:
```
cd /opt/pleora/ebus_sdk/RHEL-6-i686/bin/
./GEVPlayer
```
Then, in the GUI, set everything in the camera parameters menu to enable a fixed IP address with the desired value. If you don't touch anything, the thermocam is set to take with the address 192.168.1.41

- **Your own computer**: If you are running on Windows: Go in Control Panel/Network and Internet/Network Connections. Find your Local Area Wired Network. Click right then Properties. Select IPV4 settings then edit and set:
```
IP address : 192.168.1.4
Submask: 255.255.255.0
Gateway: /
```
Finsish with OK. Then you can plug you ethernet cable.


6. Execute:
-----------

Finally, assemble Halo. Disconnect the ethernet cable from ORF camera. Power on Halo and wait a dozen of seconds before plugging the ethernet cable back in the ORF. Verify the ORF IP address is well set and start the program:
```
test_Halo
```

8. Known issues:
-------------

During compilation:
- The defines *WORD* and *DWORD* are already defined: **You need to install the MESA-IMAGING driver correctly (step 2)**
- PLEORA_LIBS cannot be found during cmake **OR** *PvUtils.h ...etc... cannot be found* during compilation **OR** Linking error *undifined reference PvSystem ...*: **You need to install the PLEORA driver correctly (step 2)**

During execution:
- The pleora env are not sourced: **Verify the first line in your *test_Halo* script! (step 4)**
- Either the ORF or the thermocam cannot be initialized: **The IP addresses are badly configured (step 7)**
- The OpticsMount cannot be initialized: **Verify the OpticsMount IDs in /opt/GogglesOptics/CAMERA_FILE (see VERTIGO installation)**
- OpticsMount calibration parameters are not found: **Verify *extrinsic.yml* and *intrinsic.yml* files are in /opt/GogglesOptics/Calib_Params/defaultSet/ (see VERTIGO installation)**
- ORF calibrations parameters are not found: **Verify *ORF_calib.xml* is placed in the folder you specify in the parameter ORF_CALIB_FILENAME (step 5)**
- The ORF pictures have a very bad quality: **Verify visual ORF parameters**

