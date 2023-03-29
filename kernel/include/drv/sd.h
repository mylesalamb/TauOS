#ifndef SD_H
#define SD_H 1
#include <types.h>

#define SD_REGS_OFFSET 0x340000

extern struct blk_dev sd_device;

struct sd_regs {
        ureg32 arg2;                           
        ureg16 block_size;
        ureg16 block_cnt;
        ureg32 cmd_arg;                      
        ureg16 transfer_mode;
        ureg16 cmd;
        ureg32 response[4];                
        ureg32 buffer_data;
        ureg32 present_state;
        ureg8 host_control_1;
        ureg8 power_control;
        ureg8 block_gap_control;
        ureg8 wakeup_control;
        ureg16 clock_control;
        ureg8 timeout_control;
        ureg8 software_reset;
        /* Not using anything beneath this I dont think */
        ureg32 int_status;
        ureg32 int_status_enable;
        ureg32 int_signal_enable;
        ureg16 acmd_error_status;
        ureg16 host_control_2;
        ureg32 capabilities[2];
        ureg32 _reserved0[2];
        ureg32 int_force;
        ureg32 _reserved1[7];
        ureg32 boot_timeout;
        ureg32 debug_config;
        ureg32 reserved2[2];
        ureg32 ext_fifo_config;
        ureg32 ext_fifo_enable;
        ureg32 tune_step;
        ureg32 tune_SDR;
        ureg32 tune_DDR;
        ureg32 reserved3[23];
        ureg32 spi_int_support;
        ureg32 reserved4[2];
        ureg32 slot_int_status;
};

void sd_init(u64);
void sd_read(u8 *, u64);
void sd_seek(u64);
#endif
