# TauOS

An experimental operating system for the Raspberry Pi, written in C targetting
arm64.

The operating system exists merely as a collection of drivers at the moment,
although it is nearing supporting loading programs from disk.

## Building / Running

It is assumed that the host machine is x86\_64 and is running some linux distribution.
It has only been tested on Artix(runit) but should also work on other distros.

This project uses an arm64 cross compiler. To get the cross-compiler used by this project run.

```
./build.sh toolchain
```

To then build the operating system run.

```
./build.sh os 
```

To then transfer the bootloader/kernel to an SD card run.

```
sudo ./build.sh sd $DEV
```

where `$DEV` represents the name of the SD device's boot partition. This value
defaults to `mmcblk0p1`.

To then run the uart-boot loader run

```
sudo ./build.sh chain
```

You can then power on the device, and the previously built kernel will be loaded to the Pi's memory
and will be executed.


## Design Decisions

As this operating system will realistically only ever target one board, the
Raspberry Pi 4b, I will probably develop it as a monolithic kernel. As the hardware
fixed. Maybe I'll port it to the Raspberry Pi 5, or a mangopi-mq, whenever I am in
posession of one of those boards.

I have a SPI ethernet board which is much easier to program than the PCI ethernet
on the board, so networking is something that I will also work on.

## Roadmap

- [x] Mini UART output
- [x] Videocore Mailbox property API
- [x] Framebuffer / HDMI output
- [x] Drop to EL1 (non-secure)
- [x] IRQ Setup
- [x] 3rd Stage bootloader
- [x] GIC-400 Setup
- [x] Timer interrupts
- [x] DMA controller Setup
- [ ] EMMC2 sd controller setup 
- [ ] Minimal FAT32 FS driver
- [ ] SPI Ethernet (ENC28J60)
- [ ] MMU initialisation
- [ ] PMM -> VMM allocator stack (kmalloc, kcalloc, kpalloc, upalloc...)
- [ ] User processes

## Dependencies

* BASH, other shells will probably work as well
* GNU Make, I use some GNU specific stuff so other varients of Make will probably not work
* ARM cross-compiler, although you can get that through `./build.sh toolchain`
* x86 C Compiler, for tools shipped with the project

## Resources Used

* [BCM2711 Peripherals manual](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)
* [osdev Wiki](https://wiki.osdev.org/ARM_Overview)
* [rockytriton's youtube series](https://github.com/rockytriton/LLD)
