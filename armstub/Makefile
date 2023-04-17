MOD = armstub
$(eval $(call MODULE_template,$(MOD)))
$(eval $(call RULES_template,$($(MOD)_SDIR),$($(MOD)_ODIR)))

$(armstub_ODIR)armstub8.elf: $($(MOD)_SOBJS)
	$(LD) --section-start=.text=0 -o $@ $^


$(armstub_ODIR)armstub.bin: $(armstub_ODIR)armstub8.elf
	$(OC) $^ -O binary $@

armstub: $(armstub_ODIR)armstub.bin