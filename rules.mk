include ${BUILD_ROOT}/dirs.mk
SRC_DIR=$(shell pwd)
OBJ_DIR=$(subst ${SRC_ROOT},${OBJ_ROOT},$(shell pwd))
C_FILES=$(realpath $(wildcard *.c))
AS_FILES=$(realpath $(wildcard *.S))
O_FILES=$(patsubst ${SRC_DIR}/%.c,${OBJ_DIR}/%.c.o,${C_FILES})
O_FILES+=$(patsubst ${SRC_DIR}/%.S,${OBJ_DIR}/%.S.o,${AS_FILES})

${OBJ_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.c.o: ${SRC_DIR}/%.c | ${OBJ_DIR}
	@echo "[CC]: $(shell basename $<) -> $(shell basename $@)"
	${CC} ${C_FLAGS} -c $^ -o $@  ${I_FLAGS}
${OBJ_DIR}/%.S.o: ${SRC_DIR}/%.S | ${OBJ_DIR}
	@echo "[AS]: $(shell basename $<) -> $(shell basename $@)"
	@${CC} ${C_FLAGS} -c $^ -o $@ ${I_FLAGS}
all: ${O_FILES}

