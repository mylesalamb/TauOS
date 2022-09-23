#!/usr/bin/env bash

# Cross compiler should be on the path so we can use the commands normally
BUILD_ROOT=`pwd`
CC_DIR="gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf"
CROSS_CC="${BUILD_ROOT}/${CC_DIR}/bin"
PATH=$CROSS_CC:$PATH
COMPILER_SRC="https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/10.3-2021.07/binrel/${CC_DIR}.tar.xz"

export BUILD_ROOT
export PATH

_DEFAULT_OS_TARGET='all'
_DEFAULT_SD_TARGET='mmcblk0p1'


sync_sd () {
        mount /dev/$1 /tmp/mnt
        cp obj/bootloader/client/kernel8.img /tmp/mnt
        cp obj/armstub/armstub.bin /tmp/mnt
        cp bios/config.txt /tmp/mnt
        sync
        umount /tmp/mnt
}

boot_from_host () {
        echo "Start development bootloader"
        ./obj/bootloader/host/hostloader --input ./obj/kernel/kernel8.img

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
                        make ${1:-$_DEFAULT_OS_TARGET} -{r,R}
                        ;;
                "chain")
                        boot_from_host
                        ;;
                "sd")
                        sync_sd ${1:-$_DEFAULT_SD_TARGET}
                        ;;
        esac
}
set -x
main $@
