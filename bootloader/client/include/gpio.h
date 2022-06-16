#ifndef GPIO_H
#define GPIO_H 1

#include <types.h>

#define PHY_OFFSET_GPIO_REG (u64)0x200000

typedef enum _pinfn {
        gf_input = 0,
        gf_output = 1,
        gf_alt0 = 4,
        gf_alt1 = 5,
        gf_alt2 = 6,
        gf_alt3 = 7,
        gf_alt4 = 3,
        gf_alt5 = 2

} pinfn;

struct _gpio_reg {
        ureg32 reserved;
        ureg32 data[2];
};

struct gpio_regs {
        ureg32  GPFSEL[6];
        struct _gpio_reg GPSET;
        struct _gpio_reg GPCLR;
        struct _gpio_reg GPLEV;
        struct _gpio_reg GPEDS;
        struct _gpio_reg GPREN;
        struct _gpio_reg GPFEN;
        struct _gpio_reg GPHEN;
        struct _gpio_reg GPLEN;
        struct _gpio_reg GPAREN;
        struct _gpio_reg GPAFEN;
        // Weirdly big unused bit here?
        // Likely something undocumented
        ureg32 _reserved[22];
        ureg32 GPIO_PUP_PDN_CNTRL_REG[4];
};

void gpio_enable(u8);
void gpio_setfunc(u8, pinfn);
struct gpio_regs *gpio_init(u64);

#endif
