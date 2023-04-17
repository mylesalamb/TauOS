$(eval $(call MODULE_template,client))
$(eval include $(ROOTDIR)/mkconfig)


I_FLAGS = -I$(client_SDIR)/include
C_FLAGS = -Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only
L_FLAGS = -lgcc
LINK_FLAGS = -T $(client_SDIR)/config/linker.ld

$(eval $(call RULES_template,$(client_SDIR),$(client_ODIR)))

$(client_ODIR)kernel8.elf: $(client_COBJS) $(client_SOBJS) 
	${CC} $(LINK_FLAGS) -o $@ ${C_FLAGS} $^ ${L_FLAGS}

$(client_ODIR)kernel8.img: $(client_ODIR)kernel8.elf
	${OC} $^ -O binary $@

client: $(client_ODIR)kernel8.img
	@echo "Built bootloader for $(GENERAL_BOARD)"

