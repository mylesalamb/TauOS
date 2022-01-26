all: kernel armstub

.PHONY: kernel armstub clean
kernel:
	$(MAKE) all -C kernel/
armstub: 
	$(MAKE) all -C armstub/

clean:
	-rm -rf obj/
