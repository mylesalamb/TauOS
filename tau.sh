#!/usr/bin/env bash
#tau.sh: TauOS build script
#        most operating system projects
#        dont have an easy way to build the project
#        so I have written a handy collection of functions
#        to automate setting up... dependencies
#        SD cards, pulling firmware.

# Cross compiler should be on the path so we can use the commands normally
BUILD_ROOT=`pwd`
CC_DIR="gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf"
CROSS_CC="${BUILD_ROOT}/${CC_DIR}/bin"
PATH=$CROSS_CC:$PATH
COMPILER_SRC="https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/10.3-2021.07/binrel/${CC_DIR}.tar.xz"

FIRMWARE_REVISION='3b0b0c730dd09415978214a1e60ceb37ec47ca80'
FIRMWARE_REPO="https://raw.githubusercontent.com/raspberrypi/firmware/$FIRMWARE_REVISION/boot/"
FIRMWARE_FILES='fixup4.dat start4.elf bcm2711-rpi-4-b.dtb'
FIRMWARE_CACHE="$BUILD_ROOT/.fw"

export BUILD_ROOT
export PATH

_DEFAULT_OS_TARGET='all'
_DEFAULT_DEV_TARGET='/dev/mmcblk0'
_DEFAULT_SD_TARGET='mmcblk0p1'

main() {
        local cmd=$1
        shift

        case $cmd in
                "build")
                        _tau_build $@
                        ;;
                "get")
                        _tau_get $@
                        ;;
                "dist")
                        _tau_dist $@
                        ;;
                "run")
                        _tau_run $@
                        ;;
                "--help")
                        echo -e "Usage: tau.sh [options]\n"
                        echo "Options;"
                        echo -e "\tbuild\tBuilds a requested resource"
                        echo -e "\tget\tGets a requested resource"
                        echo -e "\tdist\tDistributes a requested resource"
                        echo -e "\trun\tRuns a requested resource"
                        ;;
                *)
                        echo "Command not recognised! ($cmd)"
                        exit 1
                        ;;
        esac
}

_tau_build () {
        local cmd=$1
        shift

        case $cmd in
                "os")
                        make ${1:-$_DEFAULT_OS_TARGET} -{r,R,s}
                        generate_symbol_table
                        ;;
                "sd")
                        {
                                echo "o"; # Create a new dos/mbr partition table
                                echo "n"; # Create a new partion
                                echo "p"; # Primary
                                echo "1"; #
                                echo "";
                                echo "+128M"

                                echo "n";
                                echo "p";
                                echo "2";
                                echo "";
                                echo "";

                                echo "t";
                                echo "1";
                                echo "0c";

                                echo "t";
                                echo "2";
                                echo "83";

                                echo "w";


                        } | fdisk $_DEFAULT_DEV_TARGET
                        mkfs.fat -F 32 "${_DEFAULT_DEV_TARGET}p1" 
                        mkfs -t ext2 "${_DEFAULT_DEV_TARGET}p2" 
                        ;;
                *)
                        echo "Subcommand not recognised ($cmd)"
                        exit 1
                        ;;



                esac


        }

        _tau_get () {
                local cmd=$1
                shift

                case $cmd in
                        "firmware")
                                mkdir $FIRMWARE_CACHE
                                for file in $FIRMWARE_FILES; do
                                        [ ! -f "$FIRMWARE_CACHE/$file" ] && wget "$FIRMWARE_REPO/$file" -P "$FIRMWARE_CACHE" 
                                done
                                ;;
                        "toolchain")
                                wget "${COMPILER_SRC}"
                                tar -xvf $CC_DIR.tar.xz
                                ;;
                        *)
                                echo "Subcommand not recognised ($cmd)"
                                exit 1
                                ;;
                esac


        }

        _tau_dist () {
                local cmd=$1
                shift

                mnt=${1:-$_DEFAULT_SD_TARGET}


                case $cmd in
                        "os")
                                echo "Copying kernel to remote"
                                mount /dev/$mnt /tmp/mnt
                                cp obj/kernel/kernel8.img /tmp/mnt
                                cp obj/armstub/armstub.bin /tmp/mnt
                                cp bios/config.txt /tmp/mnt
                                sync
                                umount /tmp/mnt
                                ;;
                        "bootloader")
                                echo "Copying bootloader to remote"
                                mount /dev/$mnt /tmp/mnt
                                cp obj/bootloader/client/kernel8.img /tmp/mnt
                                cp obj/armstub/armstub.bin /tmp/mnt
                                cp bios/config.txt /tmp/mnt
                                sync
                                umount /tmp/mnt
                                ;;

                        "firmware")
                                mount /dev/$mnt /tmp/mnt
                                for file in $FIRMWARE_CACHE/*; do
                                        echo "Copying $file to remote"
                                        cp $file /tmp/mnt
                                done
                                umount /tmp/mnt
                                ;;
                        "--help")
                                echo ""
                                ;;

                        *)
                                echo "Subcommand not recognised ($cmd)"
                                exit 1
                                ;;
                esac


        }

        _tau_run () {
                local cmd=$1
                shift

                case $cmd in
                        "chain")
                                ./obj/bootloader/host/hostloader --input ./obj/kernel/kernel8.img
                                ;;
                        "qemu")
                                qemu-system-aarch64 -machine virt -m 1024M -device ramfb -kernel ./obj/kernel/kernel8.img
                                ;;
                esac
        }

        generate_symbol_table() {
                local _SYMOUT='./obj/tools/symtab/symtab.bin'
                echo "Generate symbol table to $_SYMOUT"

        # Get machine formatted symbol table output
        # replace leading spaces, replace human formatted columns with commas
        readelf -Ws ./obj/kernel/kernel8.elf | tail --lines=+4 | sed -E 's/^[[:space:]]*([[:digit:]]+):/\1/; s/[[:space:]]+/,/g' | ./obj/tools/symtab/symtab --output $_SYMOUT
}



main $@
