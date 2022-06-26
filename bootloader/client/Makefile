include ${BUILD_ROOT}/config.mk
include ${BUILD_ROOT}/rules.mk

SUBSYSTEMS=$(filter-out include/. config/., $(wildcard */.))

export I_FLAGS=-I ${SRC_DIR}/include
LINKER_DIR=${SRC_DIR}/config
EXEC_IMG=${OBJ_DIR}/kernel8.img
EXEC_ELF=${OBJ_DIR}/kernel8.elf


all: $(SUBSYSTEMS) ${EXEC_IMG}

${EXEC_IMG}: ${EXEC_ELF}
	@echo "[OC]: $@"
	@${OC} $^ -O binary $@

${EXEC_ELF}: ${LINKER_DIR}/linker.ld ${SUBSYSTEMS} ${O_FILES}
	@echo "[LD] $@"
	@${CC} -T $< -o $@ ${C_FLAGS} $(shell find ${OBJ_DIR} -type f -name "*.o") ${L_FLAGS}

.PHONY: $(SUBSYSTEMS)
$(SUBSYSTEMS):
	@echo $(SUBSYSTEMS)
	$(MAKE) all -C $@

