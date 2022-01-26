include ${BUILD_ROOT}/config.mk
include ${BUILD_ROOT}/rules.mk


EXEC_IMG=${OBJ_DIR}/armstub.bin
EXEC_ELF=${OBJ_DIR}/armstub.elf


all: ${EXEC_IMG}

${EXEC_IMG}: ${EXEC_ELF}
	${OC} $^ -O binary $@

${EXEC_ELF}: ${BUILD_DIR}/linker.ld ${SUBSYSTEMS} ${O_FILES}
	${CC} -T $< -o $@ ${C_FLAGS} $(shell find ${OBJ_DIR} -type f -name "*.o") ${L_FLAGS}

