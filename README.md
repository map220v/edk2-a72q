EDK2 for Samsung Galaxy A72.

Based on zhuowei's commits for Pixel3XL - https://github.com/Pixel3Dev/edk2-pixel3

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
