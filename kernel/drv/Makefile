include ${BUILD_ROOT}/config.mk
include ${BUILD_ROOT}/rules.mk

SUBSYSTEMS=$(filter-out include/. config/., $(wildcard */.))
.PHONY: $(SUBSYSTEMS)
$(SUBSYSTEMS):
	@$(MAKE) all -C $@

all: $(SUBSYSTEMS) $(O_FILES)
