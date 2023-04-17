MOD := host
CFLAGS := -O2

.PHONY: host

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call HOSTRULES_template,$(host_SDIR),$(host_ODIR)))

EXECUTABLE := $(host_ODIR)/hostloader

$(EXECUTABLE): $(host_COBJS)
	$(HOSTCC) -o $@ $^


host: $(EXECUTABLE)