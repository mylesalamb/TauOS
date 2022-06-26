all: kernel armstub bootloader

.PHONY: kernel bootloader armstub clean
kernel:
	$(MAKE) all -C kernel/
bootloader:
	$(MAKE) all -C bootloader/
armstub: 
	$(MAKE) all -C armstub/

clean:
	-rm -rf obj/
