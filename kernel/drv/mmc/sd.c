#include <types.h>
#include <mb.h>
#include <drv/mmc/sd.h>
#include <lib/common.h>
#include <drv/blk.h>
#include <klog.h>
#include <drv/common.h>
#include <drv/timer/timer.h>

#define SD_STATE_CMD_INHIBIT BIT(0)
#define SD_STATE_DAT_INHIBIT BIT(1)

#define SD_SW_RESET_ALL BIT(0)
#define SD_SW_RESET_MASK 0x7 /* 0b111 */

/* Clock rate for identification phase */
#define SD_CLOCK_IDENT   400000

/* Clock rate for data-transfer, max supported for legacy 3.3v signaling*/
#define SD_CLOCK_NORMAL 25000000
#define SD_CLOCK_CTRL_STABLE BIT(1)

/* Masks for commands that get special treatment */
#define SD_ACMD_MASK 0x40
#define SD_DCMD_MASK 0x80

/* Normal commands */
#define SD_CMD0   0  /* Go Idle command */
#define SD_CMD2   2  /* Send CID information */
#define SD_CMD3   3  /* Send rca (relative card address)*/
#define SD_CMD4   4  /* Set DSR */
#define SD_CMD7   7  /* Enter transfer state */
#define SD_CMD8   8  /* Send if cond */
#define SD_CMD9   9  /* Get card info */
#define SD_CMD17  17 /* Read single block*/
#define SD_CMD18  18 /* Read multiple block */
#define SD_CMD24  24 /* Write single block */
#define SD_CMD25  25 /* Write multiple block */
#define SD_CMD55 55  /* prepare app command */

/* Application commands */
#define SD_ACMD41 (SD_ACMD_MASK | 41)

#define SD_CMD_RE_TYPE_0        0x0
#define SD_CMD_RE_TYPE_136      0x1 /* R2 */
#define SD_CMD_RE_TYPE_48       0x2 /* R3, R4. CRC-enable: R1, R5, R6, R7 */
#define SD_CMD_RE_TYPE_48B      0x3 /* R1b, R5b */

/* Int status masks */
#define SD_INT_STATUS_CMD_COMPLETE      BIT(0)
#define SD_INT_STATUS_TRANSFER_COMPLETE BIT(1)
#define SD_INT_STATUS_BLK_GAP           BIT(2)
#define SD_INT_STATUS_ERROR             BIT(15)
#define SD_INT_STATUS_BUFF_READ_READY   BIT(5)
#define SD_INT_STATUS_BUFF_WRITE_READY  BIT(4)

/* Tranfer mode bits */
#define SD_TXFER_MODE_IS_READ     BIT(4)
#define SD_TXFER_MODE_MULTI_BLOCK (BIT(1) | BIT(5))

struct sd_cmd {
        u8 response_type_select : 2;
        u8 sub_command_flag : 1;
        u8 crc_check_enable : 1;
        u8 index_check_enable : 1;
        u8 data_present_select : 1;
        u8 command_type : 2;
        u8 command_index : 6;
        u8 _reserved : 2;
};

#define SD_STATE_FLAGS_IS_V2 BIT(0)
#define SD_STATE_FLAGS_IS_HCS BIT(1)
#define SD_STATE_FLAGS_APP_CMD BIT(2)


/* Model current state of the card we are using*/
struct sd_state {
        u64 flags;
        u64 offset;
        u8 cmd_idx;
        u32 rca;
        u32 response[4];
} sd_dev_state;

struct sd_cmd commands[] = {
        [SD_CMD0]  =  {0, 0, 0, 0, 0, 0, SD_CMD0, 0},
        [SD_CMD2]  =  {SD_CMD_RE_TYPE_136, 0, 0, 0, 0, 0, SD_CMD2, 0},
        [SD_CMD3]  =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 0, 0, SD_CMD3, 0},
        [SD_CMD4]  =  {0, 0, 0, 0, 0, 0, SD_CMD4, 0},
        [SD_CMD7]  =  {SD_CMD_RE_TYPE_48B, 0, 1, 0, 0, 0, SD_CMD7, 0},
        [SD_CMD8]  =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 0, 0, SD_CMD8, 0}, 
        [SD_CMD9]  =  {SD_CMD_RE_TYPE_136, 0, 0, 0, 0, 0, SD_CMD9, 0},
        [SD_CMD17] =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 1, 0, SD_CMD17, 0},
        [SD_CMD18] =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 1, 0, SD_CMD18, 0},
        [SD_CMD24] =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 1, 0, SD_CMD24, 0},
        [SD_CMD25] =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 1, 0, SD_CMD25, 0},
        [SD_CMD55] =  {SD_CMD_RE_TYPE_48,  0, 1, 0, 0, 0, SD_CMD55, 0},
        [SD_ACMD41] = {SD_CMD_RE_TYPE_48,  0, 0, 0, 0, 0, 41, 0},
};

struct sd_regs *sd_dev;

void sd_dump_regs();


void sd_wait_cmd()
{
        while(!sd_dev->int_status)
                ;


        /* Read and write clear the interrupt */
        klog_debug("interrupt status is %h\n", sd_dev->int_status);
        sd_dev->int_status |= 0x1;
}

u32 sd_get_clock_divider(u32 base_clock, u32 target_rate)
{
        /* EMMC2 on rpi4 uses the 10bit divider scheme */
        /* Host controller version is 3.0 so we can choose, but the 10bit scheme is the easiest to use*/
        /* This algorithm is 'dumb' but much more readable than the one from circle*/
        u32 divider = 1;

        while(target_rate < (base_clock / (2 * divider)))
        {
                divider++;
        }


        klog_debug("Derived divider of %d: %d / 2 * %d = %d ~= %d\n", divider, base_clock, divider, base_clock / (2* divider), target_rate);
        return divider;

}

void sd_setup_clock()
{
        /* EMMC2 clock rate is discovered from the gpu? */
        /* We use a mailbox call to discover it, and set an appropriate */
        /* Divisor on the clock control register, that appears to be what the */

        u32 emmc_clock = mb_get_clock_rate(MB_CLOCK_ID_EMMC2);
        sd_dev->host_control_2 = 0;

        u32 divider = sd_get_clock_divider(emmc_clock, SD_CLOCK_IDENT);
        u16 clock_control = sd_dev->clock_control;
        klog_debug("Clock control is %h\n", clock_control);

        /* Or in the divider to the control register*/
        clock_control &= 0x3f;
        clock_control |= divider << 8;
        clock_control |= (divider & 0x300) >> 2;
        clock_control &= ~ BIT(5);
        clock_control |= BIT(0);
        klog_debug("Clock control is %h\n", clock_control);
        sd_dev->clock_control = clock_control;


        klog_debug("Waiting for clock to become stable!\n");
        while(!(sd_dev->clock_control & SD_CLOCK_CTRL_STABLE))
                ;
        klog_debug("Clock is stable!\n");

        timer_sleep_ms(50);
        /* Supply the clock signal to the card */
        sd_dev->clock_control |= BIT(2);

}

void sd_switch_clock_rate(u32 target_rate)
{

        u32 emmc_clock = mb_get_clock_rate(MB_CLOCK_ID_EMMC2);
        u16 clock_control = sd_dev->clock_control;

        while(sd_dev->present_state & (SD_STATE_CMD_INHIBIT | SD_STATE_DAT_INHIBIT));


        sd_dev->clock_control = clock_control & (~ BIT(0));
        u32 divider = sd_get_clock_divider(emmc_clock, target_rate);
        clock_control  &= ~(0xff << 8);
        clock_control |= divider << 8;
        clock_control |= (divider & 0x300) >> 2;
        timer_sleep_ms(50);
        sd_dev->clock_control = clock_control;
        while(!(sd_dev->clock_control & SD_CLOCK_CTRL_STABLE))
                ;
        
        timer_sleep_ms(50);
}

void sd_issue_norm_cmd(u32 cmd_idx, u32 arg)
{
        if(cmd_idx & SD_ACMD_MASK)
        {
                sd_issue_norm_cmd(SD_CMD55, sd_dev_state.rca);
        }
        while(sd_dev->present_state & SD_STATE_CMD_INHIBIT);

        struct sd_cmd *cmd = &commands[cmd_idx];
        sd_dev->cmd_arg = arg;
        sd_dev->transfer_mode = 0;
        sd_dev->cmd = *(u16 *)cmd;

        sd_wait_cmd();
}

void _sd_data_read(void *_buffer, u64 size)
{
        u32 *buffer = (u32 *)_buffer;
        while(size)
        {
                while(!(sd_dev->int_status & SD_INT_STATUS_BUFF_READ_READY))
                {
                        klog_debug("Int status is: %h\n", sd_dev->int_status);
                }
                u32 blk = 512;

                while(blk)
                {
                        *buffer = sd_dev->buffer_data;
                        buffer++;
                        blk -= 4;
                }
                size--;
        }

        klog_debug("Data read done!\n");

}


void _sd_data_write(void * _buffer, u64 size)
{
        u32 *buffer = (u32 *)_buffer;
        while(1)
        {
                while(!(sd_dev->int_status & SD_INT_STATUS_BUFF_WRITE_READY));
                
                /* Hardcoded block size */
                u32 blk = 512;

                while(blk)
                {
                        *buffer = sd_dev->buffer_data;
                        buffer++;
                        blk -= 4;
                }

        }
}

void sd_issue_data_command(u32 cmd_idx, u32 arg)
{
        u8 is_read = 0;
        u8 is_multi = 0;

        switch(cmd_idx){
                case SD_CMD17:
                case SD_CMD18:
                        klog_debug("Command is read\n");
                        is_read = 1;
                        break;
                case SD_CMD24:
                case SD_CMD25:
                        break;
                default:
                        klog_warn("Command %h is not a supported read or write command\n", cmd_idx);
                        return;
        }

        if(cmd_idx == SD_CMD25 || cmd_idx == SD_CMD18)
        {
                is_multi = 1;
        }

        while(sd_dev->present_state & (SD_STATE_CMD_INHIBIT | SD_STATE_DAT_INHIBIT));

        if(is_read)
        {
                sd_dev->transfer_mode |= SD_TXFER_MODE_IS_READ; 
        }
        if(is_multi)
        {
                sd_dev->transfer_mode |= SD_TXFER_MODE_MULTI_BLOCK;
        }

        klog_debug("writing command reg\n");
        struct sd_cmd *cmd = &commands[cmd_idx];
        sd_dev->cmd_arg = arg;
        sd_dev->cmd = *(u16 *)cmd;

}

void sd_reset_card()
{
        sd_dev_state.rca = 0;
        /* Issue a software reset to the controller */
        sd_dev->software_reset |= SD_SW_RESET_ALL;
        while(sd_dev->software_reset & SD_SW_RESET_MASK)
        {
                klog_debug("Waiting for reset...\n");
        }

        /* Enable VDD1 to operate at 3.3v */
        sd_dev->power_control = 0xf;
        timer_sleep_ms(50);


        sd_setup_clock();

        timer_sleep_ms(50);
        while(sd_dev->present_state & SD_STATE_CMD_INHIBIT);
        sd_dev->int_status_enable = 0xffffffff;


        timer_sleep_ms(50);

        /* Clear interrupts we recieved */
        u32 int_status = sd_dev->int_status;
        sd_dev->int_status = int_status;


        sd_issue_norm_cmd(SD_CMD0, 0);


        /* Do we have a card we can work with */
        klog_debug("Check card suitability...\n");
        sd_issue_norm_cmd(SD_CMD8, 0x1AA);
        if ((sd_dev->response[0] & 0xfff) != 0x1aa)
        {
                klog_debug("Didnt get expected response\n");
                return;
        }
        sd_dev_state.flags |= SD_STATE_FLAGS_IS_V2;
        klog_debug("card supports supplied voltage!\n");


        sd_issue_norm_cmd(SD_ACMD41, 0x0);
        u32 ocr = sd_dev->response[0];
        klog_debug("Read ocr as %h\n", ocr);


        /* In the init phase, cards have an assumed RCA of 0 */
        while(1)
        {
                sd_issue_norm_cmd(SD_ACMD41, 0xFF8000 | (BIT(30)));

                u32 resp = sd_dev->response[0];
                if((resp >> 30) & 1){
                        klog_debug("Card supports sdhc!\n");
                }
                if((resp & BIT(31)))
                {
                        klog_debug("ACMD41 done!\n");
                        break;
                }
        }


        timer_sleep_ms(50);
        klog_debug("int status %h\n", sd_dev->int_status);
        sd_issue_norm_cmd(SD_CMD2, 0xff);

        klog_debug("Read card info as %h.%h.%h.%h\n", sd_dev->response[0], sd_dev->response[1], sd_dev->response[2], sd_dev->response[3]);

        sd_issue_norm_cmd(SD_CMD3, 0);

        sd_dev_state.rca = (sd_dev->response[0] >> 16) & 0xffff;
        klog_debug("Read card RCA as %h\n", sd_dev_state.rca);


        klog_debug("Card state is %h\n", (sd_dev->response[0] >> 8) & 0xf);

        sd_issue_norm_cmd(SD_CMD9, sd_dev_state.rca << 16 | 0xffff );
        klog_debug("Card status descr: %h, %h, %h, %h\n", sd_dev->response[0], sd_dev->response[1], sd_dev->response[2], sd_dev->response[3]);

        sd_issue_norm_cmd(SD_CMD7, sd_dev_state.rca << 16 | 0xffff);
        /* std isnt particularly clear when youre supposed to do this */
        sd_switch_clock_rate(SD_CLOCK_NORMAL);
        //timer_sleep_ms(1000);
}

void sd_write(u8 *buffer, u64 size)
{
        (void)buffer;
        (void)size;
        return;
}

void sd_read(u8 *buffer, u64 size)
{
        u32 cmd;
        u32 lba = sd_dev_state.offset;
        u32 block_count = (size / 512) + ((size % 512) ? 1 : 0);

        //klog_debug("Ensure card is in data state\n");
        // sd_issue_norm_cmd(SD_CMD7, sd_dev_state.rca << 16 | 0xffff);


        if(block_count == 1)
        {
                klog_debug("Block count is 1, executing SD_CMD17\n");
                cmd = SD_CMD17;
        }
        else
        {
                klog_debug("Block count is >1, executing SD_CMD18\n");
                cmd = SD_CMD18;
        }
        sd_dev->block_size = 512;
        sd_dev->block_cnt = block_count;

        sd_issue_data_command(cmd, lba);
        _sd_data_read(buffer, block_count);
        klog_debug("done!\n");

        sd_wait_cmd();

}

void sd_seek(u64 offset)
{
        /* Set the byte offset into the card */
        sd_dev_state.offset = offset;
}

void sd_init(u64 base_addr)
{
        sd_dev = (base_addr + SD_REGS_OFFSET);
        sd_reset_card();
}

void sd_dump_regs()
{
        DUMP_OFFSET(struct sd_regs, arg2);                           
        DUMP_OFFSET(struct sd_regs, block_size);
        DUMP_OFFSET(struct sd_regs, block_cnt);
        DUMP_OFFSET(struct sd_regs, cmd_arg);                      
        DUMP_OFFSET(struct sd_regs, transfer_mode);
        DUMP_OFFSET(struct sd_regs, cmd);
        DUMP_OFFSET(struct sd_regs, response);                
        DUMP_OFFSET(struct sd_regs, buffer_data);
        DUMP_OFFSET(struct sd_regs, present_state);
        DUMP_OFFSET(struct sd_regs, host_control_1);
        DUMP_OFFSET(struct sd_regs, power_control);
        DUMP_OFFSET(struct sd_regs, block_gap_control);
        DUMP_OFFSET(struct sd_regs, wakeup_control);
        DUMP_OFFSET(struct sd_regs, clock_control);
        DUMP_OFFSET(struct sd_regs, timeout_control);
        DUMP_OFFSET(struct sd_regs, software_reset);
        DUMP_OFFSET(struct sd_regs, int_status);
        DUMP_OFFSET(struct sd_regs, int_status_enable);
        DUMP_OFFSET(struct sd_regs, int_signal_enable);
        DUMP_OFFSET(struct sd_regs, acmd_error_status);
        DUMP_OFFSET(struct sd_regs, host_control_2);
        DUMP_OFFSET(struct sd_regs, capabilities);
        DUMP_OFFSET(struct sd_regs, int_force);
        DUMP_OFFSET(struct sd_regs, boot_timeout);
        DUMP_OFFSET(struct sd_regs, debug_config);
        DUMP_OFFSET(struct sd_regs, ext_fifo_config);
        DUMP_OFFSET(struct sd_regs, ext_fifo_enable);
        DUMP_OFFSET(struct sd_regs, tune_step);
        DUMP_OFFSET(struct sd_regs, tune_SDR);
        DUMP_OFFSET(struct sd_regs, tune_DDR);
        DUMP_OFFSET(struct sd_regs, spi_int_support);
        DUMP_OFFSET(struct sd_regs, slot_int_status);
}


struct blk_dev sd_device = {
        .seek = sd_seek,
        .read = sd_read,
        .write = sd_write,
};

struct tauos_device_compat emmc2_compat[] = {
        { .compatible = "brcm,bcm2711-emmc2" },
        {}
};

TAU_DRIVER(emmc2) = {
        .name = "Broadcom EMMC2 driver",
        .dclass = DCLASS_MMC,
        .compat = emmc2_compat
};
