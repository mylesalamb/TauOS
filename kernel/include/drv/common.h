#ifndef DRV_H
#define DRV_H 1

/* Associate something with a particular linker section */
#define __lga_attrib(section_name) \
        __attribute__((unused, section(".lga_"#section_name)))

/* define some value at an appropriate linker section */
#define lga_el(type, array_name, element_name) \
        type __lga_##array_name##_##element_name __lga_attrib(array_name##_3__element_name)

#define lga_get_array(array_name) \
	({ \
		static char a[0] __lga_attrib(array_name##_1); \
		(void*)&a; \
	})

#define _lga_get_array(type, array_name, variable_name) \
	static char __lga_##variable_name[0] __lga_attrib(array_name##_1); \
	type *variable_name = (type*)&__lga_##variable_name;
#define _lga_get_array_static(type, array_name, variable_name) \
	static char __lga_##variable_name[0] __lga_attrib(array_name##_1); \
	static type *variable_name = (type*)&__lga_##variable_name;

#define lga_get_array_end(array_name) \
	({ \
		static char a[0] __lga_attrib(array_name##_4); \
		(void*)&a; \
	})

#define lga_get_array_size(array_name) \
	({ \
		void *s = lga_get_array(array_name); \
		void *e = lga_get_array_end(array_name); \
		size_t size = e - s; \
		size; \
	})

#define lga_get_array_length(type, array_name) \
	({ \
		size_t len = lga_get_array_size(array_name); \
		len /= sizeof(type); \
		len; \
	})

#endif
