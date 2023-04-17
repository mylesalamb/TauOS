MOD := bconfig
CFLAGS := -O2

.PHONY: bconfig

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call HOSTRULES_template,$(bconfig_SDIR),$(bconfig_ODIR)))

EXECUTABLE := $(bconfig_ODIR)/bconfig

$(EXECUTABLE): $(bconfig_COBJS)
	$(HOSTCC) -o $@ $^


bconfig: $(EXECUTABLE)