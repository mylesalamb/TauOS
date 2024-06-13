MOD := kernel

I_FLAGS = -I$(kernel_SDIR)/include
C_FLAGS = -Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only -g -pie
L_FLAGS = -lgcc

.PHONY: kernel

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call RULES_template,$(kernel_SDIR),$(kernel_ODIR)))



kernel_SUBS := boot
$(foreach module,$(kernel_SUBS),$(eval include $(kernel_SDIR)/$(module)/Makefile))

EXEC_BIN := $(kernel_ODIR)/kernel.img
EXEC_ELF := $(kernel_ODIR)/kernel.elf
LINK_FLAGS := -T $(kernel_SDIR)/linker.ld


$(EXEC_ELF): $(kernel_COBJS) $(kernel_SOBJS) 
	$(CC) $(LINK_FLAGS) -o $@ $(C_FLAGS) $^ $(L_FLAGS)

$(EXEC_BIN): $(EXEC_ELF)
	$(OC) $^ -O binary $@


kernel: $(EXEC_BIN)
