#include <printk.h>
#include <mm/mmu.h>

#define PG_TABLE_ENTRIES 512

void mmu_dump(pg_table *pgd)
{
	printk("Dumping page table entries at: %p\n", pgd);

	for (int i = 0; i < PG_TABLE_ENTRIES * 2; i++) {
		pg_table e = pgd[i];
		if (e) {
			printk("%d: %x\n", i, e);
		}
	}
}
