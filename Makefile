# TauOS top level makefile


MODULES 	:= kernel initrd
# Various config directories
export ROOTDIR 		:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
export MKDIR 		:= $(ROOTDIR)mk/
export OBJDIR 		:= $(ROOTDIR)obj/
export BINDIR		:= $(ROOTDIR)bin/
export ARMCCDIR		:= $(ROOTDIR)bin/cross-cc/bin/
export CCPREFIX		:= $(ARMCCDIR)aarch64-none-elf
export CC 			:= $(CCPREFIX)-gcc
export AS 			:= $(CCPREFIX)-as
export LD 			:= $(CCPREFIX)-ld
export OC			:= $(CCPREFIX)-objcopy

export HOSTCC 		:= /usr/bin/gcc
export FIND 		:= /usr/bin/find
export XARGS		:= /usr/bin/xargs
export INDENT 		:= /usr/bin/indent
export QEMU			:= /usr/bin/qemu-system-aarch64
all: $(MODULES)

# END of templates, the default all rull should just be all of the modules
.PHONY: $(MODULES) clean config lint

kernel:
	$(MAKE) -C $(CURDIR)/$@/ $@

initrd:
	$(MAKE) -C $(CURDIR)/$@/ $@

run:
	$(QEMU) -machine virt -m 256M -kernel $(OBJDIR)kernel/kernel.img -cpu cortex-a53 -nographic -initrd $(OBJDIR)initrd/initrd.tar -append foo=bar
debug:
	$(QEMU) -s -S -machine virt -m 256M -kernel $(OBJDIR)kernel/kernel.img -cpu cortex-a53 -nographic -initrd $(OBJDIR)initrd/initrd.tar


clean:
	rm -rf $(OBJDIR)
	$(FIND) $(ROOTDIR) -name '*.[c,h]~' -exec rm {} \;

lint:
	$(FIND) $(ROOTDIR)/kernel -name '*.[c,h]' | $(XARGS) $(INDENT) -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

config:

	@echo "ROOTDIR  := $(ROOTDIR)"
	@echo "MKDIR    := $(MKDIR)"
	@echo "OBJDIR   := $(OBJDIR)"
	@echo "BINDIR   := $(BINDIR)"
	@echo "ARMCCDIR := $(ARMCCDIR)"
	@echo "CCPREFIX := $(CCPREFIX)"
	@echo "CC       := $(CC)"
	@echo "AS       := $(AS)"
	@echo "LD       := $(LD)"
	@echo "OC       := $(OC)"
	@echo "HOSTCC   := $(HOSTCC)"
	@echo "FIND     := $(FIND)"
	@echo "XARGS    := $(XARGS)"
	@echo "INDENT   := $(INDENT)"
	@echo "QEMU     := $(QEMU)"
