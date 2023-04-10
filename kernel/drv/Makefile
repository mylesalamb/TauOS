include ${BUILD_ROOT}/config.mk
include ${BUILD_ROOT}/rules.mk

SUBSYSTEMS=$(filter-out include/. config/., $(wildcard */.))

all: $(SUBSYSTEMS) $(O_FILES)

.PHONY: $(SUBSYSTEMS)
$(SUBSYSTEMS):
	@$(MAKE) all -C $@

