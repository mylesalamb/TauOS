#ifndef LIB_TAR_H
#define LIB_TAR_H 1

#include <stddef.h>

struct __attribute__((packed)) tar_header {
	char fname[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char check[8];
	char link;
	char link_name[100];
	char ustar[6];
	char version[2];
	char uid_name[32];
	char gid_name[32];
	char dev_major[8];
	char dev_minor[8];
	char fname_prefix[155];
};

int tar_check_header(struct tar_header *);
int tar_get_file(struct tar_header *, const char *, void **, size_t *);
int tar_dump(struct tar_header *);

#endif
