kernel_SUBS := lib fs mm
$(eval $(call MODULE_template,kernel))
$(eval include $(ROOTDIR)/mkconfig)


I_FLAGS = -I$(kernel_SDIR)/include
C_FLAGS = -Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only
L_FLAGS = -lgcc
LINK_FLAGS = -T $(kernel_SDIR)march/$(GENERAL_BOARD)/config/linker.ld

$(eval include $(kernel_SDIR)march/$(GENERAL_BOARD)/Makefile)
$(eval $(call RULES_template,$(kernel_SDIR),$(kernel_ODIR)))
$(foreach module,$(kernel_SUBS),$(eval include $(kernel_SDIR)$(module)/Makefile))

$(kernel_ODIR)kernel8.elf: $(kernel_OBJS) 
	${CC} $(LINK_FLAGS) -o $@ ${C_FLAGS} $^ ${L_FLAGS}

$(kernel_ODIR)kernel8.img: $(kernel_ODIR)kernel8.elf
	${OC} $^ -O binary $@

kernel: $(kernel_ODIR)kernel8.img
	@echo "Built kernel for $(GENERAL_BOARD)"

