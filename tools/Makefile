SUBSYSTEMS=$(wildcard */.)

all: $(SUBSYSTEMS) 

.PHONY: $(SUBSYSTEMS)
$(SUBSYSTEMS):
	@echo $(SUBSYSTEMS)
	$(MAKE) all -C $@
