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

$(1)_COBJS := $(patsubst $(ROOTDIR)%.c,$(OBJDIR)%.c.o,$($(1)_CSRCS))
$(1)_SOBJS := $(patsubst ${ROOTDIR}%.S,${OBJDIR}%.S.o,$($(1)_SSRCS))
endef

define RULES_template =
$(2)%.S.o: $(1)%.S
	@mkdir -p $$(dir $$@)
	@echo "cc: $$^ => $$@"
	@${CC} ${C_FLAGS} -c $$^ -o $$@  ${I_FLAGS}
$(2)%.c.o: $(1)%.c
	@mkdir -p $$(dir $$@)
	@echo "cc: $$(patsubst $$(ROOTDIR),"",$($$^)) => $$@"
	@${CC} ${C_FLAGS} -c $$^ -o $$@  ${I_FLAGS}
endef

define HOSTRULES_template =
$(2):
	@mkdir -p $(2)
$(2)%.c.o: $(1)%.c | $(2)
	@echo "cc: $$^ => $$@"
	@${HOSTCC} -c $$^ -o $$@ 
endef

define MODULE_template =

	$(eval $(call SDIR_template,$(1)))
	$(eval $(call ODIR_template,$(1)))

	$(eval $(call SFILES_template,$(1)))
	$(eval $(call OFILES_template,$(1)))

endef


define SYSTEM_DIRS

$(1)_SDIR := $(2)
$(1)_ODIR := $(patsubst $(ROOTDIR)%,$(OBJDIR)%,$(2))

endef

define SYSTEM_SOURCES

$(1)_CSRCS := $(realpath $(wildcard $($(1)_SDIR)/*.c))
$(1)_SSRCS := $(realpath $(wildcard $($(1)_SDIR)/*.S))

endef

define SYSTEM_OBJECTS

$(1)_COBJS := $(patsubst $(ROOTDIR)%.c,$(OBJDIR)%.c.o,$($(1)_CSRCS))
$(1)_SOBJS := $(patsubst $(ROOTDIR)%.S,$(OBJDIR)%.S.o,$($(1)_SSRCS))

endef

# Template that defines a subsystem that comprises
# part of TauOS or associated tooling
# differences are subtle to a module in that we dont
# rely on MAKEFILE_LIST, as this file is included
# we cant rely on that variable for current directory
define SYSTEM_template =

	$(eval $(call SYSTEM_DIRS,$(1),$(2)))
	$(eval $(call SYSTEM_SOURCES,$(1)))
	$(eval $(call SYSTEM_OBJECTS,$(1)))

endef