MOD := symtab
CFLAGS := -O2

.PHONY: symtab

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call HOSTRULES_template,$(symtab_SDIR),$(symtab_ODIR)))

EXECUTABLE := $(symtab_ODIR)/symtab

$(EXECUTABLE): $(symtab_COBJS)
	$(HOSTCC) -o $@ $^


symtab: $(EXECUTABLE)
