# TauOS top level makefile


MODULES 	:= tools armstub bootloader kernel userspace
# Various config directories
export ROOTDIR 		:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
export MKDIR 		:= $(ROOTDIR)mk/
export OBJDIR 		:= $(ROOTDIR)obj/
export BINDIR		:= $(ROOTDIR)bin/
export ARMCCDIR		:= $(ROOTDIR)cross-cc/bin/
export CCPREFIX		:= aarch64-none-elf
export CC 			:= $(CCPREFIX)-gcc
export AS 			:= $(CCPREFIX)-as
export LD 			:= $(CCPREFIX)-ld
export OC			:= $(CCPREFIX)-objcopy
export HOSTCC 		:= gcc
export PATH := $(ARMCCDIR):$(PATH)

all: $(MODULES)

# END of templates, the default all rull should just be all of the modules
.PHONY: $(MODULES) clean genconfig

tools:
	$(MAKE) -C $(CURDIR)/$@/ $@

armstub:
	$(MAKE) -C $(CURDIR)/$@/ $@

bootloader:
	$(MAKE) -C $(CURDIR)/$@/ $@

kernel:
	$(MAKE) -C $(CURDIR)/$@/ $@

userspace:
	$(MAKE) -C $(CURDIR)/$@/ $@



clean:
	rm -rf $(OBJDIR)