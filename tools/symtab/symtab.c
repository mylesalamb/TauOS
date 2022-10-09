#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
/*
 * utility for generating symbol tables
 * from a particular binary, this tool expects
 * output from a tool such as nm on stdin
 *
 * and will generate a binary file to a location given as
 * --output
 */

#define DEF_OUTPUT "symtab.bin"
#define BUFF_MAX 256

static struct option opts[] = {
        {"output", required_argument, NULL, 'o'},
        {0,0,0,0}
};

void parse_symtab_entry(char *, int *, uint64_t *, int *, char *, char *, char *, char *, char *);
void write_symtab_entry(FILE *, uint64_t, char *);

int main(int argc, char *argv[])
{
        int entries = 0;
        int verbose = 0;
        char *out = DEF_OUTPUT;
        char line[BUFF_MAX];
        FILE *ofile;
        printf("TauOS symbol table generator\n\n");
        for(int optin=0, curr=0; (curr=getopt_long(argc, argv, "o:", opts, &optin)) >= 0;)
        {
                switch(curr)
                {
                        case 'o':
                                out = optarg;
                                break;
                        case 'v':
                                verbose = 1;
                                break;
                        case '?':
                                fprintf(stderr, "Unrecognised argument on command line\n");
                                break;

                }

        }
        ofile = fopen(out, "wb");
        if(!ofile)
        {
                fprintf(stderr, "Failed to open output file\n");
                exit(1);
        }
        int entry_num;
        uint64_t entry_addr;
        int entry_sz;
        char entry_type[16];
        char entry_scope[16];
        char entry_vis[16];
        char entry_ndx[16];
        char entry_name[32];

        while(fgets(line, BUFF_MAX, stdin))
        {
                parse_symtab_entry(line, &entry_num, &entry_addr, &entry_sz, entry_type, entry_scope, entry_vis, entry_ndx, entry_name);
                if(strcmp(entry_type, "FUNC"))
                {
                        continue;
                }
                if(verbose)
                        printf("Write symtab entry %16s -> %lx\n", entry_name, entry_addr);

                write_symtab_entry(ofile, entry_addr, entry_name);
                entries++;
        }
        fclose(ofile);
        printf("Wrote %d entries to %s\n", entries, out);
        return 0;
}
void parse_symtab_entry(char *line, int *entry_num, uint64_t *entry_addr, int *entry_sz, char *entry_type, char *entry_scope, char *entry_vis, char *entry_ndx, char *entry_name)
{
        sscanf(line, "%d,%lx,%d,%[^,],%[^,],%[^,],%[^,],%[^,\n]\n", entry_num, entry_addr, entry_sz, entry_type, entry_scope, entry_vis, entry_ndx, entry_name);
}
void write_symtab_entry(FILE *f, uint64_t entry_addr, char *entry_name)
{

        fwrite(&entry_addr, 1, sizeof(entry_addr), f);
        fwrite(entry_name, sizeof(entry_name[0]), strlen(entry_name) + 1, f);
}
