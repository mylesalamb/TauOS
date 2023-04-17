MOD := symtab

$(eval $(call MODULE_template,$(MOD)))
$(eval $(call HOSTRULES_template,$($(MOD)_SDIR),$($(MOD)_ODIR)))



$(symtab_ODIR)symtab: $($(MOD)_OBJS)
	$(HOSTCC) -o $@ $^

symtab: $(symtab_ODIR)symtab