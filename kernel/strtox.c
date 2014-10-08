#include <kernel.h>
#include <kernel/ctype.h>
#include <kernel/strtox.h>
#include <kernel/errno.h>

static const char* detect_radix(const char *s, unsigned int *base)
{
    if (*s == '+')
        ++s;
	if (*base == 0) {
		if (s[0] == '0') {
			if (_tolower(s[1]) == 'x' && isxdigit(s[2]))
				*base = 16;
			else
				*base = 8;
		} else
			*base = 10;
	}
	if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x')
		s += 2;
	return s;
}

static unsigned long parse_integer(const char *s, unsigned int base, unsigned long *p)
{
	unsigned long res;
	unsigned int rv;
	int overflow;

	res = 0;
	rv = 0;
	overflow = 0;
	while (*s) {
		unsigned int val;
        
		if ('0' <= *s && *s <= '9')
			val = *s - '0';
		else if ('a' <= _tolower(*s) && _tolower(*s) <= 'f')
			val = _tolower(*s) - 'a' + 10;
		else
			break;
		if (val >= base)
			break;
		/*
		 * Check for overflow only if we are within range of
		 * it in the max base we support (16)
		 */
		if (__unlikely((res & (~ULONG_C(0) << (8 * __SIZEOF_LONG__ - 2))))) {
			if (res > (~ULONG_C(0) - val)/ base)
				overflow = 1;
		}
		res = res * base + val;
		rv++;
		s++;
	}
	*p = res;
	if (overflow) {
        rv |= STRTOX_OVERFLOW;
    }
	return rv;
}

int strtoul(const char *s, unsigned int base, unsigned long *res)
{
	unsigned long _res;
	unsigned int rv;
    const char* p;
	p = detect_radix(s, &base);
    rv = (p-s);
    s = p;
	rv += parse_integer(s, base, &_res);
    if (rv & STRTOX_OVERFLOW)
		return -ERANGE;
	if (rv == 0)
		return -EINVAL;
	s += rv;
    *res = _res;
	return rv;
}

int strtol(const char *s, unsigned int base, long *res)
{
	unsigned long tmp;
    int rv;
	if (s[0] == '-') {
		rv = strtoul(s + 1, base, &tmp);
		if (rv < 0)
			return rv;
		if ((long)(-tmp) >= 0)
			return -ERANGE;
		*res = -tmp;
        rv += 1;
	} else {
		rv = strtoul(s, base, &tmp);
		if (rv < 0)
			return rv;
		if ((long)tmp < 0)
			return -ERANGE;
		*res = tmp;
	}
	return rv;
}
