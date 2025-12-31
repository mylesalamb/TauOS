#include <types.h>
#include <stddef.h>
#include <mod.h>
#include <printk.h>
#include <lib/string.h>

EXPORT(printk);
EXPORT(register_console);

extern const struct exported_symbol __DATA_SYMBOLS_START[];
extern const struct exported_symbol __DATA_SYMBOLS_END[];

/**
    mod_get_exported:

    Given the name of a function, writes the address of the function to dest.

    returns 0 on success <0 otherwise
*/
int mod_get_exported(const char *name, void **dest)
{

	size_t count = __DATA_SYMBOLS_END - __DATA_SYMBOLS_START;
	printk("Have %d symbols exported\n", count);

	for (size_t i = 0; i < count; i++) {
		printk("Checking symbol %s (%p)\n",
		       __DATA_SYMBOLS_START[i].name,
		       __DATA_SYMBOLS_START[i].addr);
		if (!strcmp(name, __DATA_SYMBOLS_START[i].name)) {
			printk("Found symbol: %s\n", name);
			*dest = __DATA_SYMBOLS_START[i].addr;
			return 0;
		}
	}
	printk("Did not find symbol for name: %s\n", name);
	return -1;
}
