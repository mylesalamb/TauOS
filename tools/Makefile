
tools_SUBS := symtab bconfig
$(eval $(call MODULE_template,tools))

tools: symtab bconfig
$(foreach module,$(tools_SUBS),$(eval include $(tools_SDIR)$(module)/Makefile))
