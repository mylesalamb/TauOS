# TauOS

An experimental operating system for the Raspberry Pi, written in C targetting
arm64.

## Building / Running

It is assumed that the host machine is x86\_64 and is running some linux distribution.
It has only been tested on Artix(runit) but should also work on other distros.

To get the cross-compiler used by this project run.

```
./build.sh toolchain
```

To then build the operating system run.

```
./build.sh os 
```

To then transfer the kernal to an SD card run.

```
sudo ./build sd $DEV
```

where `$DEV` represents the name of the SD device's boot partition. This value
defaults to `mmcblk0p1`.


## Design Decisions

TODO

## Roadmap

- [x] Mini UART output
- [x] Videocore Mailbox property API
- [x] Framebuffer / HDMI output
- [x] Drop to EL1 (non-secure)
- [x] IRQ Setup
- [ ] GIC-400 Setup
- [ ] DMA controller Setup
- [ ] Arsan sd controller setup + Minimal FAT32 FS driver
- [ ] MMU initialisation
- [ ] PMM -> VMM allocator stack
