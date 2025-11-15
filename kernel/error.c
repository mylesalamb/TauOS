#include <printk.h>

void __attribute__((noreturn))_panic(const char *file, const int line,
				     const char *msg)
{
	/* No error reporting yet so just hang in a known place */
	printk("[panic, %s:%d]: %s\n", file, line, msg);
	while (1)
		asm volatile ("nop");
}
