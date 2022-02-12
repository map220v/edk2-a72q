#!/bin/bash
# based on the instructions from edk2-platform
set -e
. build_common.sh
# not actually GCC5; it's GCC7 on Ubuntu 18.04.
GCC5_AARCH64_PREFIX=aarch64-linux-gnu- build -s -n 0 -a AARCH64 -t GCC5 -p GalaxyA72/GalaxyA72_6G.dsc
gzip -c < workspace/Build/GalaxyA72/DEBUG_GCC5/FV/GALAXYA72_UEFI.fd >uefi.img
cat a72q.dtb >>uefi.img
echo > ramdisk
abootimg --create boot-a72q_DEBUG.img -k uefi.img -r ramdisk