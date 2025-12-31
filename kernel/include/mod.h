#ifndef MOD_H
#define MOD_H 1

struct exported_symbol {
	const char *name;
	void *addr;
};

#define EXPORT(fn) \
    static const struct exported_symbol __export_##fn \
    __attribute__((used, section(".data.symbols"))) = { \
        .name = #fn, \
        .addr = (void *)&fn \
    };

int mod_get_exported(const char *name, void **dest);
#endif
