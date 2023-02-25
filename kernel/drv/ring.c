#include <types.h>
#include <lib/io.h>
/* Super simple write only console for printing in the early boot process */
/* TODO: Free the page used for the ring buffer */
void ring_writes(char *);
void ring_writec(char);

#define BUFFER_SIZE 4096

extern char __write_buffer_start;
static u64 offset = 0;
static u8 overflow = 0;

struct console ring_console = {
    .writes = ring_writes
};

void ring_writes(char *s)
{
    while(*s)
    {
        ring_writec(*s);
        s++;
    }
}

void ring_writec(char c)
{
    *(&__write_buffer_start + offset) = c;
    if(++offset == 4096){
        offset = 0;
        overflow = 1;
    }
}

void ring_echo(struct console *c)
{
    if(overflow)
    {
        for(u64 i = offset; i < BUFFER_SIZE; i++)
        {
            c->writec(*(&__write_buffer_start + i));
        }
    }
    for(u64 i = 0; i < offset; i++)
    {
        c->writec(*(&__write_buffer_start + i));
    }
}