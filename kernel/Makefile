MOD := kernel

I_FLAGS = -I$(kernel_SDIR)/include
C_FLAGS = -Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only
L_FLAGS = -lgcc

.PHONY: kernel

include $(MKDIR)/templates.mk
include $(ROOTDIR)/mkconfig

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call RULES_template,$(kernel_SDIR),$(kernel_ODIR)))



kernel_SUBS := lib fs mm drv
$(foreach module,$(kernel_SUBS),$(eval include $(kernel_SDIR)/$(module)/Makefile))

$(eval include $(kernel_SDIR)/march/$(GENERAL_BOARD)/Makefile)

EXEC_BIN := $(kernel_ODIR)/kernel8.img
EXEC_ELF := $(kernel_ODIR)/kernel8.elf
LINK_FLAGS := -T $(kernel_SDIR)/march/$(GENERAL_BOARD)/config/linker.ld


$(EXEC_ELF): $(kernel_COBJS) $(kernel_SOBJS) 
	$(CC) $(LINK_FLAGS) -o $@ $(C_FLAGS) $^ $(L_FLAGS)

$(EXEC_BIN): $(EXEC_ELF)
	$(OC) $^ -O binary $@


kernel: $(EXEC_BIN)
