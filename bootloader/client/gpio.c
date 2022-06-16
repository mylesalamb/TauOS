#include <types.h>
#include <gpio.h>

#define GPIO_PIN_MAX                    57
#define GPIO_FSEL_WIDTH                 3
#define GPIO_FSEL_CLR                   7
#define GPIO_PDN_CNTRL_REG_WIDTH        2
#define GPIO_PDN_CNTRL_REG_CLR          3

static struct gpio_regs *gpio_dev_addr;

void gpio_enable(u8 pin)
{
        if(pin > GPIO_PIN_MAX)
                return;

        u8 bank = pin / 16;
        u8 offset = pin % 16;
        u32 bits = (GPIO_PDN_CNTRL_REG_WIDTH * offset);
        gpio_dev_addr->GPIO_PUP_PDN_CNTRL_REG[bank] &= ~(GPIO_PDN_CNTRL_REG_CLR << bits);
}

void gpio_setfunc(u8 pin, pinfn fn)
{
        if(pin > GPIO_PIN_MAX)
                return;

        u8 bank = pin / 10;
        u8 offset = pin % 10;
        u32 reg = gpio_dev_addr->GPFSEL[bank]; 
        u32 bits = GPIO_FSEL_WIDTH * offset;

        reg &= ~(GPIO_FSEL_CLR << bits);
        reg |= fn << bits;
        gpio_dev_addr->GPFSEL[bank] = reg; 
}

struct gpio_regs *gpio_init(u64 base_addr)
{
        gpio_dev_addr = (struct gpio_regs *)(base_addr + PHY_OFFSET_GPIO_REG);
        return gpio_dev_addr;
}

