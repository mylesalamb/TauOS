include ${BUILD_ROOT}/config.mk
include ${BUILD_ROOT}/rules.mk


EXEC_IMG=${OBJ_DIR}/armstub.bin
EXEC_ELF=${OBJ_DIR}/armstub.elf

all: ${EXEC_IMG}

${EXEC_IMG}: ${EXEC_ELF}
	@echo "[OC] $@"
	@${OC} $^ -O binary $@

${EXEC_ELF}: ${O_FILES}
	@echo "[LD] $@"
	@${LD} --section-start=.text=0 -o $@ $(shell find ${OBJ_DIR} -type f -name "*.o") 


