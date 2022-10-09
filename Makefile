all: kernel armstub bootloader tools

.PHONY: kernel bootloader armstub tools clean
kernel:
	@$(MAKE) all -C kernel/
bootloader:
	@$(MAKE) all -C bootloader/
armstub: 
	@$(MAKE) all -C armstub/
tools: 
	@$(MAKE) all -C tools/
clean:
	-rm -rf obj/
