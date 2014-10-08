#include <kernel.h>
#include <kernel/errno.h>
#include <kernel/ctype.h>
#include <kernel/strtox.h>

int printf(const char* fmt, ...);
int putchar(char c);

int printf(const char* fmt, ...) { return 0; }
int putchar(char c) { return 0; }


union conv_value {
    char c;
    unsigned char  uc;
    short w;
    unsigned short uw;
    int i;
    unsigned int u;
    long l;
    unsigned long ul;
    ll_t ll;
    llu_t ull;
    float f;
    double d;
    char* s;
};

#define CONV_LEN_MOD_NONE	0
#define CONV_LEN_MOD_hh 	1
#define CONV_LEN_MOD_h 		2
#define CONV_LEN_MOD_l 		3
#define CONV_LEN_MOD_ll		4
#define CONV_LEN_MOD_j 		5
#define CONV_LEN_MOD_z 		6
#define CONV_LEN_MOD_t 		7
#define CONV_LEN_MOD_L 		8

#define CONV_STATE_NULL		0
#define CONV_STATE_FLAGS	1
#define CONV_STATE_PREC		2
#define CONV_STATE_LEN_MOD	3
#define CONV_STATE_SPEC		4
#define CONV_STATE_ERROR	5

struct conv_spec
{
    struct {
        unsigned char left_just:1;
        unsigned char sign:1;
        unsigned char space:1;
        unsigned char alt_form:1;
        unsigned char lead_zeros:1;
    } flags;

    struct {
        unsigned char width_wild:1;
        unsigned char prec_wild:1;
        unsigned char _ignore:1;
        
    } misc;
    
    unsigned int width;
    unsigned int prec;
    unsigned char len_mod;
    unsigned char spec;
    unsigned char state;
};

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
        );
}


int ksnprintf(char* buf, size_t size, const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = kvsnprintf(buf, size, fmt, ap);
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
                ++fmt;                    
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
                ++fmt;
                if (*fmt == 'h') {
                    s->len_mod = CONV_LEN_MOD_hh;
                    ++fmt;
                    } else {
                    s->len_mod = CONV_LEN_MOD_h;
                }
                break;
            case 'l':
                ++fmt;
                if (*fmt == 'l') {
                    s->len_mod = CONV_LEN_MOD_ll;
                    ++fmt;
                } else {
                    s->len_mod = CONV_LEN_MOD_l;
                }
                break;
            case 'j':
                ++fmt;
                s->len_mod = CONV_LEN_MOD_j;
                break;
            case 'z':
                ++fmt;
                s->len_mod = CONV_LEN_MOD_z;
                break;
            case 't':
                ++fmt;
                s->len_mod = CONV_LEN_MOD_t;
                break;
            case 'L':
                ++fmt;
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
    while (*fmt++) {
        fmt = scan_state(scan_flags(fmt, s), s);
        if (s->state == CONV_STATE_SPEC) {
            s->spec = *fmt++;
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
    ll_t base = 1;
    const size_t save = size;
    /* Find the largest decimal divisor */
    for(base = 1; i/base > 10; base *= 10);
    do {
        char lsd = (i/base) % 10;
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
            return 1;
        case 's':
            v.s = va_arg(ap, char*);
            return put_s(v.s, buf, size);
        case 'd':
        case 'i':
            switch (s->len_mod) {
                case CONV_LEN_MOD_hh:
                    v.c = va_arg(ap, int);
                    return put_dec(v.c, buf, size);
                case CONV_LEN_MOD_h:
                    v.w = va_arg(ap, int);
                    return put_dec(v.w, buf, size);
                case CONV_LEN_MOD_l:
                    v.l = va_arg(ap, long);
                    return put_dec(v.l, buf, size);
                case CONV_LEN_MOD_ll:
                    v.ll = va_arg(ap, ll_t);
                    return put_dec(v.ll, buf, size);
                case CONV_LEN_MOD_j:
                case CONV_LEN_MOD_z:
                case CONV_LEN_MOD_t:
                    /* FIXME:: ignored */
                default:
                    v.i = va_arg(ap, int);
                    return put_dec(v.i, buf, size);
            }
            break;
    }
    return -1;
}

int kvsnprintf(char* buf, size_t size, const char* fmt, va_list ap_in)
{
    va_list ap;
    /* union conv_value v; */
    struct conv_spec s;
    int ret = 0;
    printf("HERE [%s]\n", fmt);
            
    va_copy(ap, ap_in);
    while (*fmt) {
        if (*fmt == '%') {
            printf("[%s]\n", fmt);
            fmt = read_spec(fmt, &s);
            if (s.state != CONV_STATE_SPEC) {
                return -EINVAL;
            }
            
            if (s.misc.width_wild) {
                s.width = va_arg(ap, int);
            }
            if (s.misc.prec_wild) {
                s.prec = va_arg(ap, int);
            }
            
            int nr = handle_spec(&s, buf, size, ap);
            printf("SIZE=%d %d\n", size, nr);
            if (nr <= 0) {
                ret = -1;
                break;
            }
            
            buf += nr;
            size -= nr;
            
            dump_spec(&s);
            
        } else {
            *buf++ = *fmt++;
            if (--size == 0) {
                ret = -1;
                break;
            }
        }
    }
    /* *buf = 0; */
    va_end(ap);
    return ret;
}
