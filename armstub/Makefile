MOD := armstub
C_FLAGS := -Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only

.PHONY: armstub

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call RULES_template,$(armstub_SDIR),$(armstub_ODIR)))

EXEC_BIN := $(armstub_ODIR)/armstub.bin
EXEC_ELF := $(armstub_ODIR)/armstub.elf

${EXEC_BIN}: ${EXEC_ELF}
	$(OC) $^ -O binary $@

$(EXEC_ELF): $(armstub_SOBJS)
	$(LD) --section-start=.text=0 -o $@ $^

armstub: $(EXEC_BIN)