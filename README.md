
HALO SOFTWARE FOR LINUX UBUNTU 10.04
====================================

Installation:
-------------

This version integrates the core for ORF and Thermocam. To use it on Halo computer, type in a shell:
```
cd ~
git clone https://github.com/Gabs48/HaloLinux_Software
cd HaloLinux_Software
mkdir build
cd build
cmake ..
make
```

To create a script to start the software, begin with:
```
cd ~
mkdir bin
cd bin
```
Then create the script all in verifying your pleora version (for example it could be *.../ebus_sdk/RHEL-6-x86_64/...*):
```
echo source /opt/pleora/ebus_sdk/RHEL-6-i686/bin/set_puregev_env >> test_Halo
OR echo source /opt/pleora/ebus_sdk/RHEL-6-i686/bin/set_puregev_env >> test_Halo
echo cd ~/HaloLinux_Software/build/bin/ >> test_Halo
echo ./TP_2601_SaveOpticsOrfThermo ~/HaloLinux_Software pFile-nosphere.txt >> test_Halo
chmod +x test_Halo
echo export PATH=$PATH:~/bin >> ~/.bashrc
source ~/.bashrc
```

After that, you can launch the program (even after reboot) by simply typing *test_Halo* from everywhere in a shell.


Known issues:
-------------

- The defines *WORD* and *DWORD* in /usr/include/definesSR.h and usr/include/libMesaSR.h are already defined and need to have another name
- The pleora env are not sourced
- The CMakeLists use a wrong path for pleora libraries and includes (see CMakesLists in testproject and ThermoCamCore
- The IP is badly configured. The thermocam IP must be set manually to 192.168.1.41, the computer IP must be set in manual mode to 192.168.1.1, the ORF takes automatically 192.168.1.42
- The OpticsMount IDs in /opt/GogglesOptics/CAMERA_FILE are wrong
- The Calibration parameters *extrinsics.yml*  *intrinsics.yml*  *ORF_calib.xml* must be placed by default in /opt/GogglesOptics/Calib_Params/defaultSet/
- The parameters are set up badly

