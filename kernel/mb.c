#include <stddef.h>
#include <types.h>
#include <klog.h>
#include <mb.h>
#include <lib/io.h>
#include <lib/mem.h>
#include <mm/mm.h>

#define PHY_OFFSET_MB_REGS 0xB880

static struct mbox_regs *mb_regs;

/* assumption: wherever this is, it can be represented as a 32 bit ptr */
static ureg32 mb_buff[256] __attribute__((aligned(16)));

struct mbox_regs *mb_init(u64 base)
{
        mb_regs = (struct mbox_regs*)(base + PHY_OFFSET_MB_REGS);
        return mb_regs;
}

struct mbox_regs *mb_get()
{
        return mb_regs;
}

void mb_send(struct mbox_hdr *buffer, u8 channel)
{
        u32 msg = ((u64)mm_ltp(buffer)) & 0xffffffff;
        struct mbox_regs *mb = mb_get();
        while(mb->status & MBOX_WFULL)
                ;
        mb->write = (msg & 0xfffffff0) | (channel & 0xf);
}

struct mbox_hdr *mb_recv(u8 channel)
{
        u32 msg;
        struct mbox_regs *mb = mb_get();

        while(1) {
                while(mb->status & MBOX_REMPTY)
                        ;
                msg = mb->read;
                if((msg & 0xf) == channel)
                        break;
        }
        return (struct mbox_hdr *)mm_ptl((void *)((u64)msg & 0xfffffff0)) ;
}

struct mbox_hdr *mb_fmt_hdr(ureg32 *buff, u32 code)
{
        struct mbox_hdr *hdr = (struct mbox_hdr *)buff;
        hdr->size = 0;
        hdr->code = code;
        return hdr;
}


void mb_finalize_msg(void *start, void *end)
{
        /* 
         * Take a pointer to the header, and a pointer to the next byte in the buffer
         * write the end tag with padding and set the size of the overal request in the
         * header of the mailbox message, i assume that the end ptr is 32 bit aligned
         * as the tag code does that already
         */
        struct mbox_hdr *hdr = start;
        u8 *_start = start;
        ureg32 *end_tag = end;
        u8 *_end = (u8 *)(end_tag + 1);

        *end_tag = 0;
        hdr->size = _end - _start;

}

u32 *mb_append_tag(
                u32 *buff,
                u32 id,
                u32 size,
                u32 code,
                void *value,
                u64 value_sz
                )
{
        ureg8 *end;
        struct mbox_tag *tag = (struct mbox_tag *)buff;
        tag->id = id;
        tag->size = size;
        tag->code = code;
        memcpy(value, (void *)tag->value, value_sz);

        // first byte after tag 
        end = tag->value + size;

        // calculate alignment to 32 bits
        u8 align = (4 - ((u64)end & 3)) & 3;
        end += align;
        klog_info("MB message tag has end %h\n", end);
        // Return the first free byte after
        return (u32 *)end;
}

struct mbox_tag *mb_get_tag(struct mbox_hdr *hdr, u32 id)
{
        struct mbox_tag *tag = (struct mbox_tag *)hdr->tags;

        /* Not the null tag*/
        while(tag->id)
        {
                if(tag->id == id)
                        break;

                tag = (struct mbox_tag *)(((u8 *)tag->value) + tag->size);
        }
        return tag;
}

ureg32 *mb_get_buff()
{
        return mb_buff;
}

u32 mb_generic_command(u32 mbox_cmd, u32 size, u32 code, void *value, u32 value_sz)
{
        klog_debug("Generic command started\n");
        struct mbox_hdr *hdr;
        u32 *end;
        hdr = mb_fmt_hdr(mb_buff, 0);
        end = mb_append_tag((u32 *)hdr->tags, mbox_cmd, size, code, value, value_sz);
        mb_finalize_msg(hdr, end);
        klog_debug("sending\n");
        mb_send(hdr, MBOX_ARM_TO_VC);
        
        (void)mb_recv(MBOX_ARM_TO_VC);
        struct mbox_tag *csr = (struct mbox_tag *)hdr->tags;
        if(csr->id != mbox_cmd) {
                klog_warn("Did not get expected mbox_tag back\n");
                return 1;
        }
        memcpy((void *)csr->value, value, csr->size);
        klog_debug("return from generic command\n");
        return 0;

}

u32 mb_get_firmware_revision()
{
        u32 rev = 0;
        mb_generic_command(MBOX_FW_VERSION, sizeof(u32), 0, &rev, 0);
        return rev;
}

u32 mb_get_board_revision()
{
        u32 rev = 0;
        mb_generic_command(MBOX_BOARD_REV, sizeof(u32), 0, &rev, 0);
        return rev;
}


u32 mb_get_clock_rate(u32 clock_id)
{
        u32 response[2];
        response[0] = clock_id;
        mb_generic_command(MBOX_GET_CLOCK_RATE, sizeof(response), 0, response, sizeof(response[0]));
        return response[1];
}

u32 mb_get_arm_mem()
{
        u32 response[2];
        mb_generic_command(MBOX_ARM_MEM, sizeof(response), 0, response, 0);
        return response[1];
}

u64 mb_get_vc_mem()
{
        u64 response = 0;
        mb_generic_command(MBOX_VC_MEM, sizeof(response), 0, &response, 0);
        return response;

}
