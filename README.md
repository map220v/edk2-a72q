EDK2 for Samsung Galaxy A72.

Based on zhuowei's commits for Pixel3XL - https://github.com/Pixel3Dev/edk2-pixel3

## Status
Boot Windows PE: Boots with clean boot.wim(not 20h2pe_new that has sdm845 drivers)

[winpe_desktop_NODRIVERS.zip](https://drive.google.com/drive/folders/1-k4LwTuVw48e3Es_CIKPNf68CA9HXYRb?usp=sharing)

Requires drivers from Snapdragon 7C(sc7180) to make usb, etc to work.

## Working
UFS Storage (using UFSDxe from edk2-sdm845-binary)

## WARNING

**DO NOT EVER TRY TO PORT IT TO *SONY* and *GOOGLE* DEVICES**

**YOUR UFS WILL BE WIPED CLEAN!!!**

## Building
Tested on:

Ubuntu 20.04 (WSL2)

Ubuntu 18.04 arm64 (android chroot)

Setup
```
git clone https://github.com/map220v/edk2-a72q
git clone https://github.com/tianocore/edk2.git --recursive --depth 1
sudo apt install build-essential uuid-dev iasl git nasm python3-distutils gcc-aarch64-linux-gnu abootimg
cd edk2-a72q
./firstrun.sh
```
Build
```
./build.sh
```
Or this for 8Gb RAM model
```
./build_8G.sh
```
Flash
```
heimdall flash --BOOT boot-a72q.img
```

# Credits

SimpleFbDxe screen driver is from imbushuo's [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg).

`fxsheep` for his original `edk2-sagit`

`strongtz` for maintaining Renegade Project
