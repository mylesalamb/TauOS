#include <types.h>
#include <aux.h>

static struct aux_regs *aux_dev_addr; 

struct aux_regs *aux_init(u64 base_addr)
{
        aux_dev_addr = (struct aux_regs*)(base_addr + PHY_OFFSET_AUX_REGS);
        return aux_dev_addr;
}

struct aux_regs *aux_get()
{
        return aux_dev_addr;
}

