#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

/* Max size of the output buffer for writing to stdout */
#define BUFF_MAX 256

/* Max sizes for section */
#define SEC_MAX 64

int verbose = 0;

static struct option opts[] = {
        {"ini-file", required_argument, NULL, 'i'},
        {"verbose", no_argument, NULL, 'v'},
        {0,0,0,0}
};

int ini_parse(char *, void (*)(char *, char *, char *));
void ini_action(char *, char *, char *);

char *lskipws(char *);

int main(int argc, char *argv[])
{
        /* Super simple utility to take an INI file and generate */
        /* a variable file to be used with make */

        int retval;
        char *input_file = NULL;

        for(int optin=0, curr=0; (curr=getopt_long(argc, argv, "i:v", opts, &optin)) >= 0;)
        {
                switch(curr)
                {
                        case 'i':
                                input_file = optarg;
                                break;
                        case 'v':
                                verbose = 1;
                                break;
                        case '?':
                                fprintf(stderr, "Unrecognised argument on command line\n");
                                break;

                }

        }


        retval = ini_parse(input_file, ini_action);
        if(retval)
        {
                fprintf(
                        stderr,
                        "An error occured while parsing the ini file (%s) "
                        "refer to above for a specific cause of the error\n",
                        input_file
                );

                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}

#define INI_SBEGIN '['
#define INI_SEND ']'
#define INI_COMMENT ';'

int ini_parse(char *filename, void (*cb)(char *, char *, char *))
{
        char line_buffer[BUFF_MAX];
        char section[SEC_MAX] = {'\0'};

        FILE *handle = fopen(filename, "r");
        if(!handle)
        {
                fprintf(stderr, "Error whilst opening file\n");
                return 1;
        }

        while(fgets(line_buffer, BUFF_MAX, handle))
        {
                char *csr = line_buffer;
                csr = lskipws(csr);

                /* Is it a comment*/
                if(*csr == INI_COMMENT)
                        continue;

                /* If this is the beginning of a config section */
                if(*csr == INI_SBEGIN)
                {
                        char *scsr = section;
                        csr++;

                        while(*csr != INI_SEND)
                        {
                                *scsr = *csr;
                                scsr++;
                                csr++;
                        }
                        scsr++;
                        *scsr = '\0';


                        continue;
                }

                if(!section[0])
                {
                        fprintf(stderr, "Variable definition outwith section!\n");
                        fclose(handle);
                        return 1;
                }

                char *var = line_buffer;
                char *val;

                while(*csr != '=')
                        csr++;
                
                *csr = '\0';
                csr++;
                val=csr;

                cb(section, var, val);
        }



        fclose(handle);
        return 0;
}

void ini_action(char *section_name, char *variable_name, char *variable_value)
{
        char output_buffer[BUFF_MAX];
        sprintf(output_buffer, "%s_%s=", section_name, variable_name);
        for(size_t i = 0; output_buffer[i]; i++)
        {
                output_buffer[i] = toupper(output_buffer[i]);
        }
        strcat(output_buffer, variable_value);

        printf("%s", output_buffer);

}

/* Given a string skip all the prefixed whitespace */
char *lskipws(char * str)
{
        char *csr = str;
        if(!str)
                return NULL;

        while(*str && isspace(*csr))
                csr++;

        return csr;
}
