# TauOS

An operating system principally targeting Aarch64, the overall intention is to build something
vaguely Unix-like that runs on a variety of consumer Arm hardware, as well as the QEMU virt machine.

## High Level Features

* Uses Linux Kernel Boot Protocol
* Runtime kernel address patching (KASLR without the 'R')
* Rudimentary device tree parsing
* And a trivial PL011 UART driver ;)

This is my 2nd attempt at building an operating system, if you are looking for a collection of code/drivers
for the BCM2711, they can be found on [this branch](https://github.com/mylesalamb/TauOS/tree/old)