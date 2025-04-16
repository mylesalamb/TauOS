#include <types.h>
#include <stddef.h>

#include <endian.h>
#include <error.h>
#include <printk.h>
#include <lib/fdt.h>
#include <lib/string.h>

struct errtabent fdt_errtab[] = {
	ERRTABENT(FDT_E_BADMAGIC),
	ERRTABENT(FDT_E_BADALIGN),
	ERRTABENT(FDT_E_BADVERSION),
	ERRTABENT(FDT_E_NOTAG),
	ERRTABENT(FDT_E_NONODE),
	ERRTABENT(FDT_E_NOADDR),
	ERRTABENT(FDT_E_NOPROP),
};

#define FDT_EARLIEST_VERSION    16
#define FDT_LATEST_VERSION      17

#define FDT_TAG_SIZE sizeof(u32)
#define FDT_TAG_ALIGN(x) ((x + (4 - 1)) & ~(4-1))

#define FDT_READ_U32(h, o) (beth32((u32 *) _fdt_offset_ptr((h), (o))))

int fdt_errtab_size = ERRTABSIZE(fdt_errtab);

/* Given a header and offset into the structure block, return a pointer to the location in the fdt */
static inline void *_fdt_offset_ptr(const struct fdt_header *h,
				    const uint offset)
{
	return (char *)h + beth32(&h->struct_offset) + offset;
}

static inline char *_fdt_offset_string(const struct fdt_header *h,
				       const uint offset)
{
	return (char *)h + beth32(&h->string_offset) + offset;
}

/* Given a node offset, return the address of the node if it exists */
int fdt_node_address(const struct fdt_header *h, const uint offset, void **addr)
{
	char *n, c;
	uintptr_t a;

	n = (char *)_fdt_offset_ptr(h, offset + FDT_TAG_SIZE);
	a = 0;
	while (*n && *n != '@')
		n++;

	if (*n == '\0')
		return -FDT_E_NOADDR;
	n++;
	while (*n) {
		if (*n >= '0' && *n <= '9') {
			c = *n - '0';
		} else if (*n >= 'a' && *n <= 'f') {
			c = *n - 'a' + 10;
		} else {
			return -FDT_E_NOADDR;
		}

		a |= c;
		a <<= 4;
		n++;
	}

	*addr = (void *)a;
	return 0;

}

/**
	fdt_check_header - Verifies the placement and contents of a flattened device tree (FDT)

	@h: a pointer to a FDT header

	returns: 0 iff the header is valid and correctly aligned
*/
int fdt_check_header(const struct fdt_header *h)
{

	if ((uintptr_t) h & (8 - 1)) {
		return -FDT_E_BADALIGN;
	}
	if (beth32(&h->magic) != FDT_HEADER_MAGIC) {
		return -FDT_E_BADMAGIC;
	}

	if (beth32(&h->version) < FDT_EARLIEST_VERSION) {
		return -FDT_E_BADVERSION;
	}

	if (beth32(&h->version_compat) > FDT_LATEST_VERSION) {
		return -FDT_E_BADVERSION;
	}

	return 0;
}

static int _fdt_get_next_tag(const struct fdt_header *h, uint o, uint *n)
{

	u32 t;
	t = FDT_READ_U32(h, o);

	switch (t) {
	case FDT_BEGIN_NODE:
		o += FDT_TAG_SIZE;
		while (*(char *)_fdt_offset_ptr(h, o++) != '\0') ;
		break;
	case FDT_NODE_PROP:
		/* Extract the length of the prop and increment */
		o += FDT_TAG_SIZE;
		o += FDT_READ_U32(h, o) + sizeof(struct fdt_prop);
		break;
	case FDT_END_NODE:
	case FDT_NODE_NOP:
		o += FDT_TAG_SIZE;
		break;
	case FDT_END:
		return -FDT_E_NOTAG;
		break;
	}

	o = FDT_TAG_ALIGN(o);
	*n = o;

	return 0;
}

void fdt_traverse(const struct fdt_header *h)
{
	int r;
	u32 t;
	uint c, n;

	c = 0;
	n = 0;
	r = 0;

	while (r >= 0) {
		t = beth32((u32 *) _fdt_offset_ptr(h, c));
		switch (t) {
		case FDT_BEGIN_NODE:
			printk("Enter node: %s offset(%d)\n",
			       (char *)_fdt_offset_ptr(h, c + FDT_TAG_SIZE), c);
			break;
		case FDT_NODE_PROP:
			printk("Property name: %s\n",
			       _fdt_offset_string(h,
						  beth32(_fdt_offset_ptr
							 (h,
							  c +
							  FDT_TAG_SIZE * 2))));
			break;
		case FDT_END_NODE:
			printk("Exit node\n\n");
			break;
		case FDT_NODE_NOP:
			break;
		case FDT_END:
			break;

		}
		r = _fdt_get_next_tag(h, c, &n);
		c = n;
	}
}

int fdt_nodeparent(const struct fdt_header *h, const uint offset, uint *parent)
{
	int cd, r, d;
	u32 t;
	uint c, n;

	/* Root node has no parent */
	if (!offset)
		return -FDT_E_NONODE;

	cd = fdt_node_depth(h, offset);

	while (r >= 0 && offset > c) {
		t = beth32((u32 *) _fdt_offset_ptr(h, c));

		switch (t) {
		case FDT_BEGIN_NODE:
			if (d < cd) {
				*parent = c;
			}
			d++;
			break;
		case FDT_END_NODE:
			d--;
			break;
		}
		r = _fdt_get_next_tag(h, c, &n);
		c = n;
	}

	return r;
}

int fdt_getprop(const struct fdt_header *h, const uint offset, const char *name,
		void **b, u32 *len)
{
	int r;
	u32 t;
	uint c, n;

	struct fdt_prop *p;
	c = offset;
	n = 0;
	r = 0;

	while (r >= 0) {
		t = beth32((u32 *) _fdt_offset_ptr(h, c));
		switch (t) {
		case FDT_NODE_PROP:
			p = _fdt_offset_ptr(h, c + FDT_TAG_SIZE);
			if (!strcmp
			    (name,
			     _fdt_offset_string(h, beth32(&p->name_offset)))) {
				*b = p + 1;
				*len = beth32(&p->length);
				return 0;
			}
			break;
		case FDT_END_NODE:
			return -FDT_E_NOPROP;
			break;
		case FDT_BEGIN_NODE:
		case FDT_NODE_NOP:
		case FDT_END:
			break;

		}
		r = _fdt_get_next_tag(h, c, &n);
		c = n;
	}

	return r;
}

int fdt_findprop_offset(const struct fdt_header *h, const uint offset,
			const char *name, uint *prop)
{
	int r;
	u32 t;
	uint c, n;

	c = offset;
	n = 0;
	r = 0;

	while (r >= 0) {
		t = beth32((u32 *) _fdt_offset_ptr(h, c));
		switch (t) {
		case FDT_NODE_PROP:
			if (!strcmp
			    (name,
			     _fdt_offset_string(h,
						beth32(_fdt_offset_ptr
						       (h,
							c +
							FDT_TAG_SIZE * 2))))) {
				*prop = c;
				return 0;
			}
			break;
		case FDT_END_NODE:
			return -FDT_E_NOPROP;
			break;
		case FDT_BEGIN_NODE:
		case FDT_NODE_NOP:
		case FDT_END:
			break;

		}
		r = _fdt_get_next_tag(h, c, &n);
		c = n;
	}

	return r;
}

const char *fdt_strerror(const int err)
{
	const char *s;
	if (err >= 0)
		return "<No error>";

	if (-err < fdt_errtab_size) {
		s = fdt_errtab[-err].str;
		if (s)
			return s;

	}

	return "<Unknown error>";
}

int fdt_next_node(const struct fdt_header *h, uint *offset, uint *depth)
{
	int r;
	u32 t;
	uint c, n;

	c = *offset;

	do {
		r = _fdt_get_next_tag(h, c, &n);
		c = n;

		t = FDT_READ_U32(h, c);
		switch (t) {
		case FDT_BEGIN_NODE:
			(*depth)++;
			*offset = c;
			return 0;
		case FDT_END_NODE:
			(*depth)--;
			break;
		}
	} while (r >= 0);

	return r;
}

/**
	fdt_node_depth:

	Given an offset to a FDT_BEGIN_NODE tag, return the depth of that node in the FDT

	@offset: the offset of the node to find the depth of

	returns:
	
	The depth of the node in the FDT, where the root node is assumed to have a depth of 0
	if an error is encountered a negative value is returned 

*/
int fdt_node_depth(const struct fdt_header *h, const uint offset)
{
	int r, d;
	u32 t;
	uint c, n;

	c = 0;
	n = 0;
	r = 0;
	d = 0;

	while (r >= 0) {
		t = beth32((u32 *) _fdt_offset_ptr(h, c));
		switch (t) {

		case FDT_BEGIN_NODE:
			if (c == offset)
				return d;
			d++;
			break;
		case FDT_END_NODE:
			d--;
			break;
		case FDT_NODE_PROP:
		case FDT_NODE_NOP:
		case FDT_END:
			break;

		}
		r = _fdt_get_next_tag(h, c, &n);
		c = n;
	}

	return r;
}

int fdt_first_child(const struct fdt_header *h, const uint offset, uint *child)
{
	int r;
	uint cd, d;
	uint csr = offset;

	cd = fdt_node_depth(h, offset);
	d = cd;

	r = fdt_next_node(h, &csr, &d);

	/* We ran into some error || no more nodes */
	if (r < 0) {
		return r;
	}

	/* Next node is at the same level, not a child */
	if (d == cd) {
		return -FDT_E_NONODE;
	}

	/* Otherwise the next next node is lower in the tree */
	*child = csr;
	return 0;
}

int fdt_next_child(const struct fdt_header *h, const uint offset, uint *next)
{

	int r;
	uint cd, d;
	uint csr = offset;

	cd = fdt_node_depth(h, offset);
	d = cd;

	while (!(r = fdt_next_node(h, &csr, &d))) {
		if (d == cd) {
			*next = csr;
			return 0;
		}

		/* We have exited the subtree rooted at parent(offset) */
		if (cd > d) {
			return -FDT_E_NONODE;
		}

		/* Child of the previous node */
	}

	return r;
}

int fdt_nodename_eq(const struct fdt_header *h, uint offset, const char *a,
		    size_t n)
{
	int r;
	const char *b = (char *)_fdt_offset_ptr(h, offset + FDT_TAG_SIZE);
	if ((r = strncmp(a, b, n)))
		return r;

	return !(!b[n] || (b[n] == '@'));
}

/**
	fdt_nodepath_offset: 
	
	Given an absolute path to a node, return the offset to the node at that
	path if it exists

	@h: Pointer to the FDT
	@path: The absolute path of the node to retrieve
	@offset: Output argument for the returned offset of the node

	Returns:
	
	Returns the offset of the node, in @offset, if an error is encountered a value <0 is returned
*/
int fdt_nodepath_offset(const struct fdt_header *h, const char *path,
			uint *offset)
{
	uint csr = 0;
	uint child_offset;
	const char *c = path;
	const char *n;
	size_t l;
	int r;

	if (*c != '/')
		return -FDT_E_NONODE;

	while (*c == '/') {

		/* Initialise, and find the substring we are looking for */
		n = strchr(c + 1, '/');
		l = n - (c + 1);
		/* Root or double slash */
		if (!l)
			break;

		c++;

		for (r = fdt_first_child(h, csr, &child_offset);;
		     r = fdt_next_child(h, child_offset, &child_offset)) {

			if (r < 0) {
				return r;
			}

			if (!fdt_nodename_eq(h, child_offset, c, l)) {
				csr = child_offset;
				c = n;
				break;
			}

		}

		c = n;
	};

	*offset = csr;

	return 0;
}

/**
	fdt_cell: 
	
	Given an offset to a BEGIN_NODE tag, and a property name 
	returns the value of the property in that node

	@h: Pointer to the FDT
	@offset: Offset to the node to retrieve the property from
	@name: Name of the property to retrieve the value from

	Returns:
	
	The value of the property if it exists, the property is assumed to be of a u32 type
	and has a small value, if the property cannot be found a value <0 is returned 
*/
int fdt_cell(const struct fdt_header *h, const uint offset, const char *name)
{
	int r;
	uint prop;

	r = fdt_findprop_offset(h, offset, name, &prop);
	if (r < 0) {
		return r;
	}
	return FDT_READ_U32(h, prop + FDT_TAG_SIZE * 3);
}

/**
	fdt_address_cells: 
	
	Given an offset to a BEGIN_NODE tag, 
	returns the value in the address-cells property of that node

	@h: Pointer to the FDT
	@offset: Offset to the node to retrieve the property from

	Returns:
	
	The value of the address-cells property if it exists, otherwise returns 2
	If an error is encountered a value <0 is returned
*/
int fdt_address_cells(const struct fdt_header *h, const uint offset)
{
	int r;

	r = fdt_cell(h, offset, "#address-cells");

	if (r < 0)
		return 2;

	return r;

}

/**
	fdt_size_cells: 
	
	Given an offset to a BEGIN_NODE tag, 
	returns the value in the size-cells property of that node

	@h: Pointer to the FDT
	@offset: Offset to the node to retrieve the property from

	Returns:
	
	The value of the size-cells property if it exists, otherwise returns 1
	If an error is encountered a value <0 is returned
*/
int fdt_size_cells(const struct fdt_header *h, const uint offset)
{
	int r;

	r = fdt_cell(h, offset, "#size-cells");

	if (r < 0)
		return 1;

	return r;
}
