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
./tau.sh get toolchain
```

To then build the operating system run.

```
./tau.sh build os 
```

If you are using an SD card with an existing install of Raspberry Pi OS, you can
transfer the bootloader to the card with.

```
sudo ./tau.sh dist bootloader
```
To run the development bootloader run.

```
sudo ./tau.sh run chain 
```

You can then power on the device, and the previously built kernel will be loaded to the Pi's memory
and will be executed. By default, the kernel is loaded to `0x80000`. Note that this
is different to the load address of `0x200000` used by the newer Raspberry Pi firmware.


If you dont have an existing install of raspberry pi on your SD card, the following set
of commands will create a Raspberry Pi compatible setup

```
./tau.sh get firmware
sudo ./tau.sh build sd
sudo ./tau.sh dist firmware
sudo ./tau.sh dist bootloader
```


## Design Decisions

I plan to develop this operating system based around a monolithic kernel as that
appears to be the easiest way to get things works in the beginning. Once the operating
system can get programs from disk, load them into memory and execute them. I plan to
port it so that the kernel will support being run on either a Raspberry Pi 4b (supported at the moment)
and the QEMU virt board for aarch64, as a means of working with an easier susbtrate for
developing functionality on the PCI bus. The Raspberry Pi 4Bs broadcom implementation
of PCI appears quite weird...

In its current state the operating system only supports SDHC SD cards, as in thats
the only the only type of SD card I have tested with, SDUC might work. SDSC definitely
will not.

I have a SPI ethernet board which is much easier to program than the PCI ethernet
on the board, so networking is something that I will also work on.

### Memory Map

At the moment, the operating system will map / use only the first gigabyte of
memory as that gives the most simple memory map to work with, and realistically
this operating system wont need much to run at all.

```
- 0x0000000000000000 - 0x0000ffffffffffff Userspace (256Tb)
- 0xffff000000000000 - 0xffff0000C0000000 Kernel logical map (1Gb)
...
```

I map the PMM bitmap immediately after the kernel as there will (always?) be spaces
left in the PTE that maps the kernel, as this appears to be a relatively easy way
to kickstart the memory management subsystem of the kernel

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
- [x] EMMC2 sd controller setup 
- [x] MMU initialisation (identity mapping)
- [x] MMU initialisation (higher-half)
- [x] PMM (Bitmap page allocator)
- [ ] VMM allocator stack (kmalloc, kcalloc, kpalloc, upalloc...)
- [ ] Minimal FAT32 FS driver
- [ ] User processes
- [ ] Refactor / Port to qemu virt board
- [ ] SPI Ethernet (ENC28J60)

## Dependencies

* BASH, other shells will probably work as well
* GNU Make, I use some GNU specific stuff so other varients of Make will probably not work
* ARM cross-compiler, although you can get that through `./tau.sh get toolchain`
* x86 C Compiler, for tools shipped with the project

## Resources Used

* [BCM2711 Peripherals manual](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)
* [osdev Wiki](https://wiki.osdev.org/ARM_Overview)
* [rockytriton's youtube series](https://github.com/rockytriton/LLD)
