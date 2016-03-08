#ifndef _SNPRINTF_H
#define _SNPRINTF_H

#include <kernel.h>
#include <kernel/types.h>

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
    
    int width;
    int prec;
    unsigned char len_mod;
    unsigned char spec;
    unsigned char state;
#ifdef TEST
    char fmt[256];
    int fmt_idx;
#endif
};

extern int vsnprintf_spec(struct conv_spec* s, char* buf, size_t size, const char* fmt, va_list ap_in)
    __printf(4,0);

extern int snprintf_spec(struct conv_spec* s, char* buf, size_t size, const char* fmt,  ...)
    __printf(4,5);

extern int snprintf(char* buf, size_t size, const char* fmt, ...)
    __printf(3,4);

extern int vsnprintf(char* buf, size_t size, const char* fmt, va_list args)
    __printf(3,0);

#endif /* _SNPRINTF_H */
