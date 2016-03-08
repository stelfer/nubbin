
#include <kernel/snprintf.h>
#include <kernel/errno.h>
#include <kernel/ctype.h>
#include <kernel/strtox.h>

int printf(const char* fmt, ...);
int putchar(char c);


#ifndef TEST
int printf(const char* fmt, ...) { return 0; }
int putchar(char c) { return 0; }
#endif

static const char* CONV_STATE(int state) {
    switch(state) {
        case CONV_STATE_NULL: return "NULL";
        case CONV_STATE_FLAGS: return "FLAGS";
        case CONV_STATE_PREC: return "PREC";
        case CONV_STATE_LEN_MOD: return "LEN_MOD";
        case CONV_STATE_SPEC: return "SPEC";
        default:
            break;
    }
    return "ERROR";
}

void dump_spec(struct conv_spec* s) {
    printf("\tflags.left_just=%d\n"
           "\tflags.sign=%d\n"
           "\tflags.space=%d\n"
           "\tflags.alt_form=%d\n"
           "\tflags.lead_zeros=%d\n"
           "\tmisc.with_wild=%d\n"
           "\tmisc.prec_wild=%d\n"
           "\twidth=%d\n"
           "\tprec=%d\n"
           "\tlen_mod=%d\n"
           "\tspec=%c\n"
           "\tstate=%s\n"
#ifdef TEST
           "\tfmt=%s\n"
#endif
           , s->flags.left_just
           , s->flags.sign
           , s->flags.space
           , s->flags.alt_form
           , s->flags.lead_zeros
           , s->misc.width_wild
           , s->misc.prec_wild
           , s->width
           , s->prec
           , s->len_mod
           , s->spec
           , CONV_STATE(s->state)
#ifdef TEST
           , s->fmt
#endif
        );
}


#ifdef TEST
#define incr_fmt(f, s) (s->fmt[s->fmt_idx++] = *fmt++)
#else
#define incr_fmt(f, s) *fmt++
#endif

int snprintf(char* buf, size_t size, const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return ret;
}

int snprintf_spec(struct conv_spec* s, char* buf, size_t size, const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf_spec(s, buf, size, fmt, ap);
    va_end(ap);
    return ret;
}


static inline const char* scan_flags(const char* fmt, struct conv_spec *s)
{
    switch(*fmt) {
        case '-':
            s->flags.left_just = 1;
            break;
        case '+':
            s->flags.sign = 1;
            break;
        case ' ':
            s->flags.space = 1;
            break;
        case '#':
            s->flags.alt_form = 1;
            break;
        case '0':
            s->flags.lead_zeros = 1;
            break;
        case '*':
            if (s->state == CONV_STATE_PREC) {
                s->misc.prec_wild = 1;
                s->state = CONV_STATE_LEN_MOD;
                incr_fmt(fmt, s);
            } else {
                s->misc.width_wild = 1;
            }
            break;
        case '.':
            s->state = CONV_STATE_PREC;
            break;
    }
    return fmt;
}

static inline const char* scan_len_mod(const char* fmt, struct conv_spec *s)
{
    if (s->state == CONV_STATE_LEN_MOD) {
        switch (*fmt) {
            case 'h':
                incr_fmt(fmt, s);
                if (*fmt == 'h') {
                    s->len_mod = CONV_LEN_MOD_hh;
                    incr_fmt(fmt, s);
                    } else {
                    s->len_mod = CONV_LEN_MOD_h;
                }
                break;
            case 'l':
                incr_fmt(fmt, s);
                if (*fmt == 'l') {
                    s->len_mod = CONV_LEN_MOD_ll;
                    incr_fmt(fmt, s);
                } else {
                    s->len_mod = CONV_LEN_MOD_l;
                }
                break;
            case 'j':
                incr_fmt(fmt, s);
                s->len_mod = CONV_LEN_MOD_j;
                break;
            case 'z':
                incr_fmt(fmt, s);
                s->len_mod = CONV_LEN_MOD_z;
                break;
            case 't':
                incr_fmt(fmt, s);
                s->len_mod = CONV_LEN_MOD_t;
                break;
            case 'L':
                incr_fmt(fmt, s);
                s->len_mod = CONV_LEN_MOD_L;
                break;
        }
        s->state = CONV_STATE_SPEC;
    }
    return fmt;
}

static inline const char* scan_state(const char* fmt, struct conv_spec* s)
{
    switch(s->state) {
        case CONV_STATE_FLAGS:
            if (isdigit(*fmt)) {
                unsigned long len;
                int rv = strtoul(fmt, 10, &len);
                if (rv <=0) {
                    s->state = CONV_STATE_ERROR;
                    return fmt;
                }
                s->width = len;
#ifdef TEST
                memcpy(&s->fmt[s->fmt_idx], fmt, rv - 1);
                s->fmt_idx += rv - 1;
#endif
                fmt += rv - 1;
            } else {
                /* We let it go around once more for '*'. Otherwise,
                 * we're done, since a '.' would have already been
                 * caught
                 */
                printf("HERE %x %c\n", *fmt, *fmt);
                if (*fmt != '*')
                    s->state = CONV_STATE_LEN_MOD;
            }
            break;
        case CONV_STATE_PREC:
            if (isdigit(*fmt)) {
                unsigned long len;
                int rv = strtoul(fmt, 10, &len);
                if (rv <=0) {
                    s->state = CONV_STATE_ERROR;
                    return fmt;
                }
                s->prec = len;
#ifdef TEST
                memcpy(&s->fmt[s->fmt_idx], fmt, rv);
                s->fmt_idx += rv;
#endif
                fmt += rv;
                s->state = CONV_STATE_LEN_MOD;
            }
            break;
    }
    return scan_len_mod(fmt, s);
}

const char* read_spec(const char* fmt, struct conv_spec* s)
{

    memset(s, 0, sizeof(struct conv_spec));

    if (*fmt != '%') {
        return fmt;
    }

    s->state = CONV_STATE_FLAGS;
    while(incr_fmt(fmt, s)) {
    /* while(*fmt++) { */
        fmt = scan_state(scan_flags(fmt, s), s);
        if (s->state == CONV_STATE_SPEC) {
            s->spec = incr_fmt(fmt, s);
            //*fmt++;
            break;
        }
    }
    return fmt;
}

static int put_s(const char* s, char* buf, size_t size)
{
    size_t save = size;
    while(*s && size > 0) {
        *buf = *s;
        ++buf;
        ++s;
        --size;
    }
    *buf = 0;
    return save - size;
}

static int put_dec(ll_t i, char* buf, size_t size)
{

    printf("PUT_DEC %d\n", i);
    
    ll_t base = 1;
    const size_t save = size;
    /* Find the largest decimal divisor */
    for(base = 1; i/base > 10; base *= 10);
    do {
        char lsd = (i/base) % 10;
        printf("i=%d base=%d base=%d\n", i, base, lsd);
        base /= 10;
        *buf = (char)(lsd + 0x30);
        ++buf;
        --size;
        if (size == 0)
            return -1;
        
    } while(base != 0);
    return save - size;
}

static int handle_spec(const struct conv_spec* s, char* buf, size_t size, va_list ap)
{
    if (size == 0) {
        return 0;
    }
    
    union conv_value v;
    switch(s->spec) {
        case 'c':
            v.c = va_arg(ap, int);
            *buf = v.c;
            printf("XCCCCC\n");
            
            return 1;
        case 's':
            v.s = va_arg(ap, char*);
            printf("SSSSS=[%s]\n", v.s);
            return put_s(v.s, buf, size);
        case 'd':
        case 'i':
            switch (s->len_mod) {
                case CONV_LEN_MOD_hh:
                    v.c = va_arg(ap, int);
                    printf("hhDEC=[%d]\n", v.c);
                    return put_dec(v.c, buf, size);
                case CONV_LEN_MOD_h:
                    v.w = va_arg(ap, int);
                    printf("hDEC=[%d]\n", v.w);
                    return put_dec(v.w, buf, size);
                case CONV_LEN_MOD_l:
                    v.l = va_arg(ap, long);
                    printf("lDEC=[%d]\n", v.l);
                    return put_dec(v.l, buf, size);
                case CONV_LEN_MOD_ll:
                    v.ll = va_arg(ap, ll_t);
                    printf("llDEC=[%d]\n", v.ll);
                    return put_dec(v.ll, buf, size);
                case CONV_LEN_MOD_j:
                case CONV_LEN_MOD_z:
                case CONV_LEN_MOD_t:
                    /* FIXME:: ignored */
                default:
                    v.i = va_arg(ap, int);
                    printf("DEC=[%d]\n", v.i);
                    return put_dec(v.i, buf, size);
            }
            break;
    }
    return -1;
}

int vsnprintf_spec(struct conv_spec* s, char* buf, size_t size, const char* fmt, va_list ap_in)
{
    const char* pbuf = buf;
    
    va_list ap;
    int ret = 0;
    printf("HERE [%s]\n", fmt);
            
    va_copy(ap, ap_in);
    while (*fmt) {
        if (*fmt == '%') {
            printf("[%s]\n", fmt);
            fmt = read_spec(fmt, s);
            if (s->state != CONV_STATE_SPEC) {
                return -EINVAL;
            }
            if (s->misc.width_wild) {
                s->width = va_arg(ap, int);
            }
            if (s->misc.prec_wild) {
                s->prec = va_arg(ap, int);
            }
            
            int nr = handle_spec(s, buf, size, ap);
            printf("SIZE=%d %d\n", size, nr);
            if (nr <= 0) {
                ret = -1;
                break;
            }
            
            buf += nr;
            size -= nr;

            printf("buf[%s]\n", pbuf);
            
            dump_spec(s);


            
        } else {
            *buf++ = *fmt++;
            if (--size == 0) {
                ret = -1;
                break;
            }
        }
    }
    va_end(ap);
    return ret;

}

int vsnprintf(char* buf, size_t size, const char* fmt, va_list ap)
{
    struct conv_spec s;
    return vsnprintf_spec(&s, buf, size, fmt, ap);
}
