#include "sprintf.hpp"

#include <libm/memStuff.h>
#include <libm/cstrTools.h>

#include <stdarg.h>

int print_num(long x, int base, char* str) {
	const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

	char buf[65];

	char* p;

	if(base > 36) {
		return 0;
	}


	bool sign = false;

	if (x < 0) {
		sign = true;
		x *= -1;
	}

	p = buf + 64;
	*p = '\0';

	do {
		*--p = digits[x % base];
		x /= base;
	} while(x);

	if (sign) {
		*--p = '-';
	}

	_memcpy(p, str, StrLen(p));

	return StrLen(p);
}

int print_num_unsigned(unsigned long x, int base, char* str) {
	const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

	char buf[65];

	char* p;

	if(base > 36) {
		return 0;
	}


	p = buf + 64;
	*p = '\0';

	do {
		*--p = digits[x % base];
		x /= base;
	} while(x);

	_memcpy(p, str, StrLen(p));

	return StrLen(p);
}

int vsprintf(char *buf, const char *str, va_list args) {
	const char* s;
	unsigned long n;
	char* orig_buf = buf;

	while(*str) {
		if(*str == '%') {
			str++;
			switch(*str) {
				case 's':
					s = va_arg(args, char*);
					while (*s) {
						*buf++ = *s++;
					}
					break;
				case 'c':
					n = va_arg(args, int);
					*buf++ = n;
					break;
				case 'd':
				case 'u':
					n = va_arg(args, long int);
					buf += print_num(n, 10, buf);
					break;
				case 'x':
				case 'p':
					n = va_arg(args, unsigned long int);
					buf += print_num_unsigned(n, 16, buf);
					break;
				case '%':
					*buf++ = '%';
					break;
				case '\0':
					goto out;
					break;
				default:
					*buf++ = '%';
					*buf++ = *str;
					break;
			}
		} else {
			*buf++ = *str;
		}
		str++;
	}
	*buf = 0;

out:
	return StrLen(orig_buf);
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}