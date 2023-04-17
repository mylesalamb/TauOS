MOD := bconfig

$(eval $(call MODULE_template,$(MOD)))
$(eval $(call HOSTRULES_template,$($(MOD)_SDIR),$($(MOD)_ODIR)))



$(bconfig_ODIR)bconfig: $($(MOD)_OBJS)
	$(HOSTCC) -o $@ $^

bconfig: $(bconfig_ODIR)bconfig