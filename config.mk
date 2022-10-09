CC_PREFIX=aarch64-none-elf
CC=${CC_PREFIX}-gcc
AS=${CC_PREFIX}-as
OC=${CC_PREFIX}-objcopy
LD=${CC_PREFIX}-ld
C_FLAGS=-Wall -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only
L_FLAGS=-lgcc
