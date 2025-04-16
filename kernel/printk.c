#include <stdarg.h>
#include <stdbool.h>
#include <types.h>
#include <printk.h>

#define _PRINT_LEFT_ALIGN   (1<<0)
#define _PRINT_INC_SIGN     (1<<1)
#define _PRINT_EXSIGN_CHAR  (1<<2)
#define _PRINT_IMSIGN_CHAR  (1<<3)
#define _PRINT_HEX_PREFIX   (1<<4)
#define _PRINT_CAPS_CHAR    (1<<5)
#define _PRINT_ZERO_PAD     (1<<6)

/* Some function to call when there are calls to printk */
static void (*_writes)(const char *);

/* Basic functions for printk parsing */
static inline bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

static inline int skip_atoi(const char **s)
{
	int retval = 0;

	while (is_digit(**s)) {
		retval *= 10;
		retval += **s - '0';
		(*s)++;
	}

	return retval;

}

char *ntos(char *dest, long num, int base, int width, unsigned int flags)
{
	/* TODO: Implement flags for numeric types */
	(void)flags;
	int i;
	char sign, t[66];
	const char *digits;
	if (base < 2 || base > 36) {
		*dest++ = '0';
		return dest;
	}
	if (flags & _PRINT_CAPS_CHAR) {
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	} else {
		digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	}
	/* Default to no sign unless some flag passed in */
	sign = '\0';
	if (flags & _PRINT_INC_SIGN) {
		if (num < 0) {
			sign = '-';
			num *= -1;
		} else {
			if (flags & _PRINT_EXSIGN_CHAR)
				sign = '+';
			if (flags & _PRINT_IMSIGN_CHAR)
				sign = ' ';
		}
	}
	i = 0;
	if (num == 0) {
		t[i++] = '0';
	} else {
		while (num > 0) {
			t[i++] = digits[num % base];
			num /= base;
		}
	}
	if (flags & _PRINT_HEX_PREFIX) {
		if (base == 16) {
			t[i++] = 'x';
			t[i++] = '0';
		} else if (base == 8) {
			t[i++] = 'o';
			t[i++] = '0';

		}

	}
	if (sign)
		t[i++] = sign;

	while (i-- > 0)
		*dest++ = t[i];

	return dest;
}

void register_console(void (*fn)(const char *s))
{
	_writes = fn;
}

int vsprintf(char *d, const char *fmt, va_list va)
{

	unsigned int flags;
	unsigned long n;
	int width;
	char *b, q;
	const char *s;
	int base;
	for (b = d; *fmt; fmt++) {
		if (*fmt != '%') {
			*b++ = *fmt;
			continue;
		}

		/* Parse the flags for the printf call */
		flags = 0;
 flags:
		fmt++;
		switch (*fmt) {
		case '-':
			flags |= _PRINT_LEFT_ALIGN;
			goto flags;
		case '+':
			flags |= _PRINT_EXSIGN_CHAR;
			goto flags;
		case ' ':
			flags |= _PRINT_IMSIGN_CHAR;
			goto flags;
		case '#':
			flags |= _PRINT_HEX_PREFIX;
			goto flags;
		case '0':
			flags |= _PRINT_ZERO_PAD;
			goto flags;

		}

		width = -1;
		/* Parse the width field for the call */
		if (is_digit(*fmt)) {
			width = skip_atoi(&fmt);
		} else if (*fmt == '*') {
			width = va_arg(va, int);
			fmt++;
		}

		/* If there is a qualifier character, extract it */
		q = '\0';
		if (*fmt == 'h' || *fmt == 'l') {
			q = *fmt++;
		}

		switch (*fmt) {
		case '%':
			*b++ = '%';
			continue;
		case 's':
			s = va_arg(va, char *);
			if (!s)
				s = "<NULL>";
			while (*s)
				*b++ = *s++;
			continue;
		case 'p':
			b = ntos(b, (unsigned long)va_arg(va, void *), 16,
				 width, flags | _PRINT_HEX_PREFIX);
			continue;
		case 'c':
			*b++ = (unsigned char)va_arg(va, int);
			continue;
		case 'd':
			flags |= _PRINT_INC_SIGN;
			base = 10;
			break;

		case 'X':
			flags |= _PRINT_CAPS_CHAR;
		case 'x':
			base = 16;
			break;
		}
		if (q) {
			if (q == 'l')
				n = va_arg(va, long);
			else if (q == 'h')
				n = (short)va_arg(va, int);
		} else {
			n = va_arg(va, int);
		}
		b = ntos(b, n, base, width, flags);
	}

	*b = '\0';
	return b - d;
}

int printk(const char *fmt, ...)
{
	int retval;
	char b[256] = { 0 };
	va_list va;
	va_start(va, fmt);
	retval = vsprintf(b, fmt, va);
	va_end(va);
	_writes(b);
	return retval;
}

int sprintf(char *b, const char *fmt, ...)
{
	int retval;
	va_list va;
	va_start(va, fmt);
	retval = vsprintf(b, fmt, va);
	va_end(va);
	return retval;
}
