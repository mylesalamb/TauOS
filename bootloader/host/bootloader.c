#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/stat.h>

#define DEV "/dev/ttyUSB0"
#define BRK '\x03'
#define ACK '\x06'
#define FIN '\x04'

static struct option opts[] = {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {0,0,0,0}
};

struct termios old_tio, new_tio;
int tio_set = 0;

void sig_cb(int signum)
{
        printf("[HOST] Caught signal: %s\n", strsignal(signum));
        if(tio_set)
        {
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
        }
        exit(0);
}

void panic(char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        if(tio_set)
        {
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
        }
        exit(1);
}

int progress(uint64_t curr, uint64_t max)
{
        printf("\r");
        fflush(stdout);

        printf("[HOST] (%6.2f%%) %10lu / %lu", ((double)curr/max) * 100, curr, max);

        return 0;
}

int copy_to_dev(int dev, char *kernel)
{
        /* Copy the actual kernel to the device */
        /* We assume that the device is ready to recieve */
        /* And that for each byte sent an ACK is recieved */
        /* From the remote */
        uint64_t sz, remaining;
        struct stat file_info;
        int file = open(kernel, O_RDONLY);
        if(dev < 0 || file < 0)
                panic("Handle to copy is invalid\n");

        lseek(file, 0, SEEK_SET);
        fstat(file, &file_info);
        sz = file_info.st_size;
        remaining = sz;

        printf("[HOST] File size is %lu\n", sz);
        printf("[HOST] Send file size to remote\n");

        char resp;
        for(int i = 0; i < sizeof(sz); i++)
        {
                // Lowest order bytes first
                char chunk = (sz >> (8*i)) & 0xFF;
                if(write(dev, &chunk, 1) < 0)
                {
                        close(file);
                        panic("Write behaved unexpectadly\n");
                }
        }

        int ret;
        while( (ret = read(dev, &resp, 1)) == 0)
                ;

        if(ret < 0)
        {
                close(file);
                panic("Read behaved unexpectadly\n");
        }
        if(resp != ACK)
        {
                close(file);
                panic("Did not recieve an ack for size, got: %d\n", resp);
        }

        printf("[HOST] Communicated size to remote, send file\n");

        while(remaining )
        {
                /* Byte by byte is slow, but good enough for PoC*/
                char byte;
                if(read(file, &byte, 1) <= 0)
                {
                        close(file);
                        panic("read behaved unexpectadly\n");
                }
                if(write(dev, &byte, 1) <= 0)
                {
                        close(file);
                        panic("write behaved unexpectadly\n");
                }
                remaining--; 
                progress(sz - remaining, sz);
        }
        printf("\n[HOST] Finished sending\n");
        char fin = FIN;
        if(write(dev, &fin, 1) <= 0)
        {
                close(file);
                panic("write behaved unexpectadly\n");
        }

        close(file);
        return 0;
}

int main(int argc, char *argv[])
{
        char *kernel = NULL;
        char *dev = DEV;
        struct termios dios;
        struct sigaction action;

        fd_set rfds, wfds, efds;

        memset(&action, 0, sizeof(action));
        action.sa_handler = sig_cb;
        sigaction(SIGTERM, &action, NULL);
        sigaction(SIGINT, &action, NULL);

        printf("TauOS development mini-uart bootloader\n\n");

        for(int optin=0,curr=0; (curr=getopt_long(argc,argv, "i:o:", opts, &optin)) >= 0;)
        {
                switch(curr)
                {
                        case 'i':
                                kernel = optarg;
                                break;
                        case 'o':
                                dev = optarg;
                                break;

                        case '?':
                                panic("Argument %c is not valid, try --input/-i or --output/-o", curr);
                                break;
                }

        }

        if(!kernel | !dev)
        {
                panic("Missing required argument, --input/-i\n");
        }
        int dfd;
        if( (dfd = open(dev, O_NOCTTY | O_RDWR | O_SYNC)) < 0)
        {
                panic("Open device file failed, errno: %s\n", strerror(errno));
        }
        if(!isatty(dfd))
        {
                panic("Output is not a tty\n");
        }

        if(!isatty(STDIN_FILENO))
        {
                panic("Input is not a tty, dont use this in a pipeline\n");
        }



        if(tcgetattr(STDIN_FILENO, &old_tio))
        {
                panic("Failed to get stdin attribs, errno: %s\n", strerror(errno));
        }


        /*  Not really set, but safe to copy back */
        tio_set = 1;
        new_tio = old_tio;
        new_tio.c_lflag &= (~ICANON & ~ECHO);

        if(tcsetattr(STDIN_FILENO, TCSANOW, &new_tio))
        {
                panic("Failed to set stdin attribs, errno: %s\n", strerror(errno));
        }

        if(tcgetattr(dfd, &dios))
        {
                panic("Failed to get device attribs, errno: %s\n", strerror(errno));
        }

        dios.c_cc[VTIME] = 0;
        dios.c_cc[VMIN] = 0;

        // 8N1 mode, no input/output/line processing masks.
        dios.c_iflag = 0;
        dios.c_oflag = 0;
        dios.c_cflag = CS8 | CREAD | CLOCAL;
        dios.c_lflag = 0;

        if(cfsetispeed(&dios, B115200))
        {
                panic("Failed to set device baud rate\n");
        }

        if(cfsetospeed(&dios, B115200))
        {
                panic("Failed to set device baud rate\n");
        }

        if(tcsetattr(dfd, TCSANOW, &dios))
        {
                panic("Failed to set device attribs, errno: %s\n", strerror(errno));
        }

        int marker = 0;
        while(1)
        {
                /* Poll stdin for stuff to write to the pi: STDIN_FILENO */
                /* Poll device to get data in and out of the pi: dfd */
                int fd_max = (STDIN_FILENO > dfd ? STDIN_FILENO : dfd) + 1;

                FD_ZERO(&wfds);
                FD_ZERO(&rfds);
                FD_ZERO(&efds);

                FD_SET(STDIN_FILENO, &rfds);
                FD_SET(dfd, &rfds);

                FD_SET(STDIN_FILENO, &efds);
                FD_SET(dfd, &efds);

                if(select(fd_max, &rfds, &wfds, &efds, NULL) < 0)
                {
                        panic("Select failed, errno: %s\n", strerror(errno));
                }
                if(FD_ISSET(STDIN_FILENO, &efds))
                {
                        panic("Error on stdin\n");                                                
                }

                if(FD_ISSET(dfd, &efds))
                {
                        panic("Error on device handle\n");
                }


                if(FD_ISSET(STDIN_FILENO, &rfds))
                {
                        char c;

                        if(read(STDIN_FILENO, &c, 1) != 1)
                        {
                                panic("Read behaved unexpectadly\n");
                        }
                        if(write(dfd, &c, 1) != 1)
                        {
                                panic("write behaved unexpectadly\n");
                        }
                }

                if(FD_ISSET(dfd, &rfds))
                {
                        char c;

                        if(read(dfd, &c, 1) != 1)
                        {
                                panic("Read behaved unexpectadly\n");
                        }
                        if(c == BRK)
                        {
                                marker++;
                                if(marker == 3){
                                        printf("[HOST] Prepare to send kenrel to host\n");
                                        copy_to_dev(dfd, kernel);
                                        marker = 0;
                                }
                        }
                        else
                        {
                                if(write(STDOUT_FILENO, &c, 1) != 1)
                                {
                                        panic("write behaved unexpectadly\n");
                                }       
                        }
                }

        }



        /* Shouldnt be hit, but if we do */
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
        return 0;
}
