# TauOS top level makefile


MODULES 	:= armstub tools kernel
# Various config directories
ROOTDIR 	:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
MKDIR 		:= $(ROOTDIR)mk/
OBJDIR 		:= $(ROOTDIR)obj/
BINDIR		:= $(ROOTDIR)bin/
ARMCCDIR	:= $(ROOTDIR)cross-cc/bin/
CCPREFIX	:= aarch64-none-elf
CC 			:= $(CCPREFIX)-gcc
AS 			:= $(CCPREFIX)-as
LD 			:= $(CCPREFIX)-ld
OC			:= $(CCPREFIX)-objcopy
HOSTCC 		:= gcc

export PATH := $(ARMCCDIR):$(PATH)

all: $(MODULES)

define SDIR_template =
$(1)_SDIR := $(dir 							    \
					$(abspath 					\
						$(lastword 				\
							$(MAKEFILE_LIST) 	\
						) 						\
					) 							\
			)
endef

define ODIR_template =
$(1)_ODIR := $(patsubst $(ROOTDIR)%,$(OBJDIR)%,$($(1)_SDIR))
endef

# Given a module, discover the source files in
# the directory, this relies on MAKEFILE_LIST
# so it must be called before any includes
# are called
define SFILES_template =
$(1)_SSRCS := $(realpath 						\
			$(wildcard 							\
				$(dir 							\
					$(abspath 					\
						$(lastword 				\
							$(MAKEFILE_LIST) 	\
						) 						\
					) 							\
				)*.S 							\
			) 									\
		)
$(1)_CSRCS := $(realpath 						\
			$(wildcard 							\
				$(dir 							\
					$(abspath 					\
						$(lastword 				\
							$(MAKEFILE_LIST) 	\
						) 						\
					) 							\
				)*.c 							\
			) 									\
		)

endef

# Given a previous call to the source generation template
# Generate the appropriate object files, couldnt find a way to do
# this in one step which is kinda annoying, its something to do
# with how eval works in make
define OFILES_template =

$(1)_OBJS := $(patsubst $(ROOTDIR)%.c,$(OBJDIR)%.c.o,$($(1)_CSRCS))
$(1)_SOBJS := $(patsubst ${ROOTDIR}%.S,${OBJDIR}%.S.o,$($(1)_SSRCS))
endef

define RULES_template =
$(2)%.S.o: $(1)%.S
	@mkdir -p $$(dir $$@)
	${CC} ${C_FLAGS} -c $$^ -o $$@  ${I_FLAGS}
$(2)%.c.o: $(1)%.c
	@mkdir -p $$(dir $$@)
	${CC} ${C_FLAGS} -c $$^ -o $$@  ${I_FLAGS}
endef

define HOSTRULES_template =
$(2):
	@mkdir -p $(2)
$(2)%.c.o: $(1)%.c | $(2)
	${HOSTCC} ${C_FLAGS} -c $$^ -o $$@  ${I_FLAGS}
endef

define MODULE_template =

	$(eval $(call SDIR_template,$(1)))
	$(eval $(call ODIR_template,$(1)))

	$(eval $(call SFILES_template,$(1)))
	$(eval $(call OFILES_template,$(1)))

endef

# END of templates, the default all rull should just be all of the modules
.PHONY: $(MODULES) clean genconfig

$(ROOTDIR)mkconfig: $(bconfig_ODIR)bconfig 
	$(bconfig_ODIR)bconfig -i $(INI) > $@



genconfig: $(ROOTDIR)mkconfig
	


clean:
	rm -rf $(OBJDIR)

$(foreach module,$(MODULES),$(eval include $(module)/Makefile))