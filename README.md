# TauOS

An operating system principally targeting AArch64, the overall intention is to build something
vaguely Unix-like that runs on a variety of consumer Arm hardware, as well as the QEMU virt machine.

In terms of architecture, although early. I intend to build the system as a hybrid/micro-kernel borrowing ideas from Minix, Linux and the BSDs.

## High Level Features

* Uses Linux Kernel Boot Protocol
* Runtime kernel address patching (KASLR without the 'R')
* Rudimentary device tree parsing
* Higher-half kernel logical map derived from advertised memory in the device tree
* Kernel page allocator, implemented using a buddy allocator
* And a trivial PL011 UART driver ;)

## Things I am working on

* Kernel modules
    * To make the core kernel as generic as possible, board specific code is to be loaded dynamically from the initial ramdisk
    * To support this further, I may try to support conditional compilation s.t only a subset of modules are included

## Building

Install the cross compiler toolchain and qemu with...

```sh
./bin/get-toolchain

# For RHEL derived distros
sudo dnf install qemu-system-aarch64 libguestfs
```

Build the kernel with...

```sh
make
```

Run the kernel with

```sh
make run
```


## Other

I write about some of the ideas that I come across developing this operating system on [my website](https://mylesalamb.com/articles.html), in the hopes that they are useful for others. I try to encapsulate a particular topic in a singular commit.

This is my 2nd attempt at building an operating system, if you are looking for a collection of code/drivers
for the BCM2711, they can be found on [this branch](https://github.com/mylesalamb/TauOS/tree/old)
