#!/usr/bin/env bash

# Cross compiler should be on the path so we can use the commands normally
export BUILD_ROOT=`pwd`
CC_DIR="gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf"
CROSS_CC="${BUILD_ROOT}/${CC_DIR}/bin"
export PATH=$CROSS_CC:$PATH
export C_INCLUDE_PATH=${BUILD_ROOT}/kernel/include

COMPILER_SRC="https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/10.3-2021.07/binrel/${CC_DIR}.tar.xz"

sync_sd () {
        mount /dev/$1 /tmp/mnt
        cp obj/kernel8.img /tmp/mnt
        cp armstub/armstub.bin /tmp/mnt
        cp bios/config.txt /tmp/mnt
        sync
        umount /tmp/mnt
}

main() {
        local cmd=$1
        shift

        case $cmd in
                "toolchain")
                        wget "${COMPILER_SRC}"
                        tar -xvf $CC_DIR.tar.xz
                        ;;
                "os")
                        make ${all:-$1} -{r,R}
                        ;;
                "sd")
                        sync_sd ${mmcblk0p1:-$1}
                        ;;
                "all")
                        make all -{r,R}
        esac
}
set -x
main $@
