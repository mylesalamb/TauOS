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

static void _tar_print_field(const char *name, const char *buf, size_t len)
{
	size_t i;

	printk("%s: \"", name);
	for (i = 0; i < len && buf[i] != '\0'; i++) {
		char c = buf[i];
		if (c >= 32 && c <= 126)
			printk("%c", c);
		else
			printk(".");
	}
	printk("\"\n");
}

size_t _tar_otoi(const char *a, size_t s)
{
	size_t val = 0;
	while (s > 0 && (*a == ' ' || *a == '\0')) {
		a++;
		s--;
	}

	while (s > 0 && (*a >= '0' && *a <= '7')) {
		val = (val << 3) + (*a - '0');
		s--;
		a++;
	}

	return val;
}

struct tar_header *_tar_next_header(struct tar_header *h)
{
	uintptr_t csr = (uintptr_t) (h + 1);
	size_t size = _tar_otoi(h->size, sizeof(h->size));

	csr += size;
	csr += TAR_BLOCKSIZE - 1;
	csr &= ~(TAR_BLOCKSIZE - 1);

	return (struct tar_header *)csr;
}

/**
	_tar_fcmp:

	Very simple string comparison-esque function
	we dont match on the full path, just the filename
*/
int _tar_fcmp(struct tar_header *h, const char *f)
{
	const char *prev = h->fname;
	const char *csr = h->fname;

	while ((csr = strchr(csr, '/'))) {
		if (!*csr) {
			csr = prev;
			break;
		}

		csr++;
		prev = csr;
	}

	return strcmp(csr, f);
}

void tar_dump_header(const struct tar_header *h)
{
	_tar_print_field("fname", h->fname, sizeof(h->fname));
	_tar_print_field("mode", h->mode, sizeof(h->mode));
	_tar_print_field("uid", h->uid, sizeof(h->uid));
	_tar_print_field("gid", h->gid, sizeof(h->gid));
	_tar_print_field("size", h->size, sizeof(h->size));
	_tar_print_field("mtime", h->mtime, sizeof(h->mtime));
	_tar_print_field("check", h->check, sizeof(h->check));

	printk("link: '%c'\n", h->link);

	_tar_print_field("link_name", h->link_name, sizeof(h->link_name));
	_tar_print_field("ustar", h->ustar, sizeof(h->ustar));
	_tar_print_field("version", h->version, sizeof(h->version));
	_tar_print_field("uid_name", h->uid_name, sizeof(h->uid_name));
	_tar_print_field("gid_name", h->gid_name, sizeof(h->gid_name));
	_tar_print_field("dev_major", h->dev_major, sizeof(h->dev_major));
	_tar_print_field("dev_minor", h->dev_minor, sizeof(h->dev_minor));
	_tar_print_field("fname_prefix", h->fname_prefix,
			 sizeof(h->fname_prefix));
}

int tar_get_file(struct tar_header *h, const char *f, void **d, size_t *s)
{
	int r;

	while (!(r = tar_check_header(h))) {
		if (!_tar_fcmp(h, f)) {
			*d = (void *)(h + 1);
			*s = _tar_otoi(h->size, sizeof(h->size));
		}

		h = _tar_next_header(h);
	}
	return -1;
}

int tar_dump(struct tar_header *h)
{

	int r;
	while (1) {
		r = tar_check_header(h);
		if (r < 0)
			break;

		tar_dump_header(h);
		h = _tar_next_header(h);
	}

	return 0;
}
