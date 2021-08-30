EDK2 for Samsung Galaxy A72.

Based on zhuowei's commits for Pixel3XL - https://github.com/Pixel3Dev/edk2-pixel3

## Status
Boot Windows10 PE: error code 0xc0000017(There isn't enough memory available to create a ramdisk device)

Boot MemTest86: Test 10 disabled (countdown not working) [MemTest86.log](https://gist.githubusercontent.com/map220v/eb1ec4104751e7bfccdc29073f1f16ee/raw/f4dfd7bd04741359e3689be3a743c4da39c6bb4f/MemTest86.log)

## Working
UFS Storage (using UFSDxe from edk2-sdm845-binary)

## Broken
RAM (only 150MB~ memory free)

## WARNING

**DO NOT EVER TRY TO PORT IT TO *SONY* and *GOOGLE* DEVICES**

**YOUR UFS WILL BE WIPED CLEAN!!!**

## Building
Tested on Ubuntu 20.04 (WSL2)

Setup
```
git clone https://github.com/map220v/edk2-a72q
git clone https://github.com/tianocore/edk2.git --recursive
git clone https://github.com/tianocore/edk2-platforms.git
sudo apt install build-essential uuid-dev iasl git nasm python3-distutils gcc-aarch64-linux-gnu abootimg
cd edk2-a72g
./firstrun.sh
```
Build
```
./build.sh
```
Flash
```
heimdall flash --BOOT boot-a72q.img
```

# Credits

SimpleFbDxe screen driver is from imbushuo's [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg).

`fxsheep` for his original `edk2-sagit`

`strongtz` for maintaining Renegade Project
