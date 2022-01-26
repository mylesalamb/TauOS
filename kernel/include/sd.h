#include <types.h>

#define SD_REGS_OFFSET 0x340000

struct sd_regs {
        ureg32 arg2;                           // offset: 00
        ureg16 blcksz;                         // offset: 04
        ureg16 blckcnt;                        // offset: 06
        ureg32 cmdarg;                         // offset: 08
        ureg16 transfer_mode;                  // offset: 0c
        ureg16 cmdreg;                         // offset: 0e
        ureg32 response1;                      // offset: 10
        ureg32 response2;                      // offset: 14
        ureg32 response3;                      // offset: 18
        ureg32 response4;
        ureg32 buffer;
        ureg32 state;
        ureg8  control;
        ureg8  pwr_control;
        ureg8  block_gap;
        ureg8  wakeup_control;
        ureg16 clock_control;
        ureg8  timeout_control;
        ureg8  software_reset;
        ureg16 normal_interrupt;
        ureg16 error_interrupt;
        ureg16 normal_ier;
        ureg16 error_ier;
        ureg16 normal_status_ier;
        ureg16 error_status_ier;
        ureg16 auto_cmd_error_status_register;
        ureg16 host_control;


};
