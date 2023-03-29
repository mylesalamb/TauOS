include ${BUILD_ROOT}/rules.mk

CC=gcc
C_FLAGS=-O2 -Wall -pedantic
EXEC=${OBJ_DIR}/bconfig

all: ${EXEC}

${EXEC}: ${O_FILES}
	${CC} -o $@ ${C_FLAGS} $(shell find ${OBJ_DIR} -type f -name "*.o") 
