#include <types.h>
#include <printk.h>
#include <lib/tar.h>
#include <lib/string.h>

#define TAR_HEADER "ustar"
#define TAR_HEADER_LEN 6

#define TAR_VERSION "00"
#define TAR_VERSION_LEN 2

#define TAR_BLOCKSIZE 512

/**
    A _very_ simple sanity check on the structure of the initrd / TAR header
    the only supported format is POSIX 1003.1-1988 (ustar)

    Simply asserts that the ustar field is present and the version is as expected
*/
int tar_check_header(struct tar_header *h)
{
	if (strncmp(TAR_HEADER, h->ustar, TAR_HEADER_LEN)) {
		return -1;
	}

	if (strncmp(TAR_VERSION, h->version, TAR_VERSION_LEN)) {
		return -1;
	}

	return 0;
}
