# TauOS top level makefile

ROOTDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

export ARMCCDIR		:= $(ROOTDIR)/bin/cross-cc/bin
export CCPREFIX		:= $(ARMCCDIR)/aarch64-none-elf
export CC 			:= $(CCPREFIX)-gcc
export AS 			:= $(CCPREFIX)-as
export LD 			:= $(CCPREFIX)-ld
export OC			:= $(CCPREFIX)-objcopy

export MKDIR 		:= $(ROOTDIR)/mk
export OBJDIR 		:= $(ROOTDIR)/obj
export DSTDIR		:= $(ROOTDIR)/dist

export HOSTCC 		:= /usr/bin/gcc
export FIND 		:= /usr/bin/find
export XARGS		:= /usr/bin/xargs
export INDENT 		:= /usr/bin/indent
export QEMU			:= /usr/bin/qemu-system-aarch64


all: kernel modules initrd

### Kernel Directives

KERNEL_ROOT := $(ROOTDIR)/kernel
KERNEL_OBJ := $(OBJDIR)/kernel
KERNEL_DST := $(DSTDIR)/kernel

KERNEL_SRCDIRS := $(ROOTDIR)/kernel \
	$(ROOTDIR)/kernel/boot \
	$(ROOTDIR)/kernel/lib \
	$(ROOTDIR)/kernel/mm

KERNEL_HDIRS := $(KERNEL_ROOT)/include \
	$(KERNEL_ROOT)/include/boot \
	$(KERNEL_ROOT)/include/common \
	$(KERNEL_ROOT)/include/lib \
	$(KERNEL_ROOT)/include/mm \


KERNEL_LDSRC := $(KERNEL_ROOT)/linker.ld
KERNEL_LDOBJ := $(KERNEL_OBJ)/linker.ld

KERNEL_CSRCS := $(foreach d,$(KERNEL_SRCDIRS),$(wildcard $(d)/*.c))
KERNEL_SSRCS := $(foreach d,$(KERNEL_SRCDIRS),$(wildcard $(d)/*.S))
KERNEL_HSRCS := $(foreach d,$(KERNEL_HDIRS),$(wildcard $(d)/*.h))
KERNEL_OBJS := $(patsubst $(KERNEL_ROOT)/%.c,$(KERNEL_OBJ)/%.c.o,$(KERNEL_CSRCS)) $(patsubst $(KERNEL_ROOT)/%.S,$(KERNEL_OBJ)/%.S.o,$(KERNEL_SSRCS))
KERNEL_HOBJS := $(patsubst $(KERNEL_ROOT)/%.h,$(KERNEL_DST)/%.h,$(KERNEL_HSRCS)) 
KERNEL_OBJDIRS := $(sort $(dir $(KERNEL_OBJS)))
KERNEL_HDST := $(patsubst $(KERNEL_ROOT)/%,$(KERNEL_DST)/%,$(KERNEL_HDIRS)) 

GIT_COMMIT := $(shell git rev-parse --short HEAD)
GIT_DIRTY := $(shell test -n "$$(git status --porcelain)" && echo "-dirty")

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

KERNEL_CFLAGS = -Wall -ffreestanding -nostdlib \
			-nostartfiles -mgeneral-regs-only \
			-g -pie -DGIT_COMMIT_HASH=\"$(GIT_COMMIT)$(GIT_DIRTY)\" \
			-DKERNEL_VERSION=\"$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)\" \
			-fmacro-prefix-map=$(KERNEL_ROOT)/=

KERNEL_LFLAGS = -lgcc

$(KERNEL_OBJDIRS):
	mkdir -p $@

$(KERNEL_HDST):
	mkdir -p $@

$(KERNEL_DST):
	mkdir -p $@

$(KERNEL_LDOBJ): $(KERNEL_LDSRC)
	$(CC) -I $(KERNEL_ROOT)/include -x c $^ -E -o $@

$(KERNEL_OBJ)/%.c.o: $(KERNEL_ROOT)/%.c | $(KERNEL_OBJDIRS)
	$(CC) $(KERNEL_CFLAGS) -I $(KERNEL_ROOT)/include -c $< -o $@ $(KERNEL_LFLAGS)

$(KERNEL_OBJ)/%.S.o: $(KERNEL_ROOT)/%.S | $(KERNEL_OBJDIRS)
	$(CC) $(KERNEL_CFLAGS) -I $(KERNEL_ROOT)/include -c $< -o $@

$(KERNEL_DST)/kernel.elf: $(KERNEL_OBJS) $(KERNEL_LDOBJ) | $(KERNEL_DST)
	$(CC) -T $(KERNEL_LDOBJ) -o $@ $(KERNEL_CFLAGS) $(KERNEL_OBJS) $(KERNEL_LFLAGS)

$(KERNEL_DST)/kernel.img: $(KERNEL_DST)/kernel.elf
	$(OC) $^ -O binary $@

$(KERNEL_DST)/%.h: $(KERNEL_ROOT)/%.h | $(KERNEL_HDST)
	@echo $(KERNEL_HDST)
	cp $< $@

.PHONY: kernel
kernel: $(KERNEL_DST)/kernel.img $(KERNEL_HOBJS)

### Module directives

define collect_modules
  ifneq (,$(wildcard $(1)/Makefile))
    include $(1)/Makefile
    $(eval MODULE_OBJS += \
      $(patsubst $(MODULE_ROOT)/%,$(MODULE_OBJDIR)/%, \
        $(addprefix $(1)/,$$(obj-m)) \
      ) \
    )
  endif
endef

MODULE_ROOT = $(ROOTDIR)/modules
MODULE_DST = $(DSTDIR)/modules
MODULE_OBJDIR = $(OBJDIR)/modules
MODULE_DIRS = $(wildcard $(MODULE_ROOT)/*)
$(foreach dir,$(MODULE_DIRS),$(eval $(call collect_modules,$(dir))))
MODULE_OBJDIRS := $(sort $(dir $(MODULE_OBJS)))

MODULE_NAMES := $(notdir $(MODULE_DIRS))
MODULE_BINS  := $(addprefix $(MODULE_DST)/,$(addsuffix .kmod,$(MODULE_NAMES)))

module_objs = $(filter $(MODULE_OBJDIR)/$1/%,$(MODULE_OBJS))
define MODULE_LINK_TEMPLATE
$(MODULE_DST)/$(1).kmod: $(call module_objs,$(1))
	@mkdir -p $$(dir $$@)
	@echo "ld (module): $$@"
	$$(LD) -r -o $$@ $$^
endef

$(foreach m,$(MODULE_NAMES),$(eval $(call MODULE_LINK_TEMPLATE,$(m))))

$(MODULE_OBJDIRS):
	mkdir -p $@

$(MODULE_DST):
	mkdir -p $@


$(MODULE_OBJDIR)/%.c.o: $(MODULE_ROOT)/%.c $(KERNEL_HOBJS) | $(MODULE_OBJDIRS)
	$(CC) $(KERNEL_CFLAGS) -I $(KERNEL_DST)/include -c $< -o $@ $(KERNEL_LFLAGS)


.PHONY: modules
modules: $(MODULE_BINS)


### Initrd directives

INITRD_DST     := $(DSTDIR)/initrd
INITRD_IMG     := $(INITRD_DST)/initrd.tar
INITRD_STAGE   := $(OBJDIR)/initrd-staging

$(INITRD_DST):
	mkdir -p $@

$(INITRD_STAGE):
	mkdir -p $@

INITRD_MODULE_DIR := $(INITRD_STAGE)/modules

$(INITRD_MODULE_DIR):
	mkdir -p $@

.PHONY: initrd-stage
initrd-stage: $(MODULE_BINS) | $(INITRD_MODULE_DIR)
	cp $(MODULE_BINS) $(INITRD_MODULE_DIR)/

$(INITRD_IMG): initrd-stage | $(INITRD_DST)
	@echo "tar: $@"
	cd $(INITRD_STAGE) && \
	tar --format=ustar -cf $@ .

.PHONY: initrd
initrd: $(INITRD_IMG)

### Run commands, `make debug` specifically for attaching GDB debug proc

run:
	$(QEMU) -machine virt,gic-version=2 -m 256M \
		-kernel $(KERNEL_DST)/kernel.img \
		-cpu cortex-a53 -nographic \
		-initrd $(INITRD_IMG) \
		-append 'intc=intc,intc-gicv2.kmod timer=timer,timer-arm-generic.kmod'
debug:
	$(QEMU) -s -S -machine virt,gic-version=2 -m 256M \
		-kernel $(KERNEL_DST)/kernel.img \
		-cpu cortex-a53 -nographic \
		-initrd $(INITRD_IMG) \
		-append 'intc=intc,intc-gicv2.kmod timer=timer,timer-arm-generic.kmod'


### Generic rules
clean:
	rm -rf $(OBJDIR)
	rm -rf $(DSTDIR)
	$(FIND) $(ROOTDIR) -name '*.[c,h]~' -exec rm {} \;

lint:
	$(FIND) $(ROOTDIR)/kernel -name '*.[c,h]' | $(XARGS) $(INDENT) \
		-nbad -bap -nbc -bbo -hnl -br -brs -c33 \
		-cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 \
		-i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
		-saf -saw -ncs -nsc -sob -nfca -cp33 -ss \
		-ts8 -il1

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