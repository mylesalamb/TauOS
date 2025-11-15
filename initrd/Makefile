MOD := initrd

.PHONY: initrd

include $(MKDIR)/templates.mk

$(eval $(call SYSTEM_template,$(MOD),$(CURDIR)))
$(eval $(call RULES_template,$(kernel_SDIR),$(kernel_ODIR)))

INITRD_TAR := $(initrd_ODIR)/initrd.tar

$(INITRD_TAR): $(initrd_SDIR)/img | $(initrd_ODIR)
	tar --format=ustar -cf $@ -C $^ .

$(initrd_ODIR):
	mkdir -p $@

initrd: $(INITRD_TAR)
	