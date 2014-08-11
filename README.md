
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

To launch the test program for ORF, thermocam and opticsmount, do:
```
cd ~
mkdir bin
cd bin
echo source /opt/pleora/ebus_sdk/RHEL-6-x86_64/bin/set_puregev_env > test
echo ./~/HaloLinux_Software/buil/bin/TP_2601_SaveOpticsOrfThermo ~/HaloLinux_Software/ pFile-nosphere.txt > test
chmod +x test
echo source ~/bin > ~/.bashrc
source ~/.bashrc
```

After that, you can launch the program (even after reboot) by simply typing *test* from everywhere in a shell.


Known issues:
-------------

- The defines *WORD* and *DWORD* in /usr/include/definesSR.h and usr/include/libMesaSR.h are already defined and need to have another name
- The pleora env are not sourced
- The CMakeLists use a wrong path for pleora libraries and includes (see CMakesLists in testproject and ThermoCamCore
- The IP is badly configured. The thermocam IP must be set manually to 192.168.1.41, the computer IP must be set in manual mode to 192.168.1.1, the ORF takes automatically 192.168.1.42
- The OpticsMount IDs in /opt/GogglesOptics/CAMERA_FILE are wrong
- The Calibration parameters *extrinsics.yml*  *intrinsics.yml*  *ORF_calib.xml* must be placed by default in /opt/GogglesOptics/Calib_Params/defaultSet/

