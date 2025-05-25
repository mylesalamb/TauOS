MOD := kernel

GIT_COMMIT := $(shell git rev-parse --short HEAD)
GIT_DIRTY := $(shell test -n "$$(git status --porcelain)" && echo "-dirty")

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

I_FLAGS = -I$(kernel_SDIR)/include
C_FLAGS = -Wall -ffreestanding -nostdlib \
			-nostartfiles -mgeneral-regs-only \
			-g -pie -DGIT_COMMIT_HASH=\"$(GIT_COMMIT)$(GIT_DIRTY)\" \
			-DKERNEL_VERSION=\"$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)\"
L_FLAGS = -lgcc

.PHONY: kernel

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call RULES_template,$(kernel_SDIR),$(kernel_ODIR)))



kernel_SUBS := boot lib mm
$(foreach module,$(kernel_SUBS),$(eval include $(kernel_SDIR)/$(module)/Makefile))

EXEC_BIN := $(kernel_ODIR)/kernel.img
EXEC_ELF := $(kernel_ODIR)/kernel.elf
LINK_FLAGS := -T $(kernel_ODIR)/linker.ld

$(kernel_ODIR)/linker.ld: $(kernel_SDIR)/linker.ld
	@printf "pp: %-30s => %s\n" "$(patsubst $(ROOTDIR)%,%,$^)" "$(patsubst $(ROOTDIR)%,%,$@)"
	@$(CC) $(I_FLAGS) -x c $^ -E -o $@

$(EXEC_ELF): $(kernel_COBJS) $(kernel_SOBJS) $(kernel_ODIR)/linker.ld 
	@printf "ld: %s\n" "$(patsubst $(ROOTDIR)%,%,$@)"
	@$(CC) -T $(kernel_ODIR)/linker.ld -o $@ $(C_FLAGS) $(kernel_COBJS) $(kernel_SOBJS) $(L_FLAGS)

$(EXEC_BIN): $(EXEC_ELF)
	@printf "oc: %-30s => %s\n" "$(patsubst $(ROOTDIR)%,%,$^)" "$(patsubst $(ROOTDIR)%,%,$@)"
	@$(OC) $^ -O binary $@


kernel: $(EXEC_BIN)
