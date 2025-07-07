# TauOS

An operating system principally targeting AArch64, the overall intention is to build something
vaguely Unix-like that runs on a variety of consumer Arm hardware, as well as the QEMU virt machine.

In terms of architecture, although early. I intend to build the system as a hybrid/micro-kernel borrowing ideas from Minix, Linux and the BSDs.

## High Level Features

* Uses Linux Kernel Boot Protocol
* Runtime kernel address patching (KASLR without the 'R')
* Rudimentary device tree parsing
* And a trivial PL011 UART driver ;)

## Other

I write about some of the ideas that I come across developing this operating system on [my website](https://mylesalamb.com/articles.html), in the hopes that they are useful for others. I try to encapsulate a particular topic in a singular commit.

This is my 2nd attempt at building an operating system, if you are looking for a collection of code/drivers
for the BCM2711, they can be found on [this branch](https://github.com/mylesalamb/TauOS/tree/old)