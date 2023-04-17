#include <types.h>
#include <klog.h>
#include <dtb.h>
#include <lib/string.h>
#include <lib/common.h>
#include <lib/io.h>

char *dtb_find_node(struct dtb_header *header, const char *search_term)
{

        u64 search_length = strlen(search_term);

        u32 string_offset = btlhw(header->string_offset);
        u32 struct_offset = btlhw(header->struct_offset);
        
        char *node_name;
        char *prop_name;
        struct dtb_prop *prop = NULL;

        u32 * csr = (u32 *)(((u8 *)header) + struct_offset);
        while(btlhw(*csr) != DTB_END)
        {
                switch (btlhw(*csr))
                {
                        case DTB_BEGIN_NODE:
                                node_name = csr + 1;
                                klog_debug("Entered new node: %s\n", node_name);
                                u64 next = strlen(node_name) + 1;
                                u64 offset = (((u64)node_name + next) + (4 - 1)) & -4;

                                if(!strncmp(search_term, node_name, search_length)){
                                        klog_debug("Found node with name");
                                        return NULL;
                                }

                                // klog_debug("node length %d, skipped from %h to %h\n", next, csr, offset);
                                csr = (u32 *)offset;
                                break;

                        case DTB_END_NODE:
                                node_name = NULL;
                                csr++;
                                break;

                        case DTB_PROP:
                                prop = (struct dtb_prop *)(csr + 1);
                                u64 length = btlhw(prop->length);
                                u64 pnext = (u64)(csr + 1) + sizeof(struct dtb_prop) + length;
                                u64 poffset = (pnext + (4 - 1) )& -4;
                                csr = (u32 *)poffset;
                                break;

                        case DTB_NOP:
                                klog_debug("NOP token, skipping it!\n");
                                csr++;
                                break;
                        default:
                                klog_debug("unrecognised token: %d\n", *csr);
                                csr++;

                }
        
        }

}

/**
 * @brief Given a device tree node, return the nodes address if it exists
 * @param node: the node to get the address of
 * @return void*: the address of the node
 */
void *dtb_get_node_address(struct dtb_node *node)
{
        return NULL;
}

/**
 * @brief Given a device tree node, return a pointer to the next node in the tree
 * 
 * @param node: the node to get the child off
 * @return struct dtb_node*: pointer to the child node
 */
struct dtb_node *_dtb_get_next(struct dtb_node *node)
{
        char *node_name;
        struct dtb_prop *prop;


        switch(btlhw(node->type))
        {
                case DTB_BEGIN_NODE:
                        node_name = node + 1;
                        klog_debug("Entered new node: %s\n", node_name);
                        u64 next = strlen(node_name) + 1;
                        u64 offset = (((u64)node_name + next) + (4 - 1)) & -4;

                        // klog_debug("node length %d, skipped from %h to %h\n", next, csr, offset);
                        return (struct dtb_node *)offset;

                case DTB_END_NODE:
                        return NULL;

                case DTB_PROP:
                        prop = (struct dtb_prop *)(node + 1);
                        u64 length = btlhw(prop->length);
                        u64 pnext = (u64)(node + 1) + sizeof(struct dtb_prop) + length;
                        u64 poffset = (pnext + (4 - 1) )& -4;
                        return (struct dtb_node *)poffset;

                case DTB_NOP:
                        return ++node;
                
                default:
                        return NULL;
        }

}

void dtb_iter_node_props(struct dtb_node *node, void (*f)(char *, void *, u32))
{

        char * name;

        /* What node are we currently processing */
        struct dtb_node *ncsr;
        /* What byte are we processing*/
        char *csr;
        if(btlhw(node->type) != DTB_BEGIN_NODE)
                return;

        name = (char *)(node + 1);
        ncsr = (struct dtb_node *)((((u64)name + strlen(name) + 1) + 3) & -4);
        klog_debug("Iterating over node properties for %s\n", name);

        while(btlhw(ncsr->type) == DTB_END_NODE)
        {

        }
        
}

void dtb_iter_reserved(struct dtb_header *header, void (*cb)(u64, u64))
{
        struct dtb_mem_reserve *csr;
        u32 offset = btlhw(header->memory_offset);
        
        csr = (struct dtb_mem_reserve *)(((u8 *)header) + offset);

        while(csr->address || csr->size)
        {
                u64 address = btlw(csr->address);
                u64 len = btlw(csr->size);
                cb(address, len);
                csr++;
        }

}

void dtb_dump_reserved(struct dtb_header *header)
{
    struct dtb_mem_reserve *csr;
    u32 offset = btlhw(header->memory_offset);
    
    csr = (struct dtb_mem_reserve *)(((u8 *)header) + offset);

    while(csr->address || csr->size)
    {
        u64 address = btlw(csr->address);
        u64 len = btlw(csr->size);
        // printk("Reserve %h - %h\n", address, address + len);
        csr++;
    }
}

void dtb_dump_struct(struct dtb_header *header)
{
        u32 string_offset = btlhw(header->string_offset);
        u32 struct_offset = btlhw(header->struct_offset);
        
        char *node_name;
        char *prop_name;
        struct dtb_prop *prop = NULL;

        u32 * csr = (u32 *)(((u8 *)header) + struct_offset);
        while(btlhw(*csr) != DTB_END)
        {
                switch (btlhw(*csr))
                {
                        case DTB_BEGIN_NODE:
                                node_name = csr + 1;
                                klog_debug("Entered new node: %s\n", node_name);
                                u64 next = strlen(node_name) + 1;
                                u64 offset = (((u64)node_name + next) + (4 - 1)) & -4;
                                // klog_debug("node length %d, skipped from %h to %h\n", next, csr, offset);
                                csr = (u32 *)offset;
                                break;

                        case DTB_END_NODE:
                                node_name = NULL;
                                csr++;
                                break;

                        case DTB_PROP:
                                prop = (struct dtb_prop *)(csr + 1);
                                u64 length = btlhw(prop->length);
                                u64 pnext = (u64)(csr + 1) + sizeof(struct dtb_prop) + length;
                                u64 poffset = (pnext + (4 - 1) )& -4;
                                csr = (u32 *)poffset;
                                break;

                        case DTB_NOP:
                                klog_debug("NOP token, skipping it!\n");
                                csr++;
                                break;
                        default:
                                klog_debug("unrecognised token: %d\n", *csr);
                                csr++;

                }
        
        }
}

void dtb_init(struct dtb_header *header)
{
    dtb_dump_struct(header);
    dtb_dump_reserved(header);    
}
