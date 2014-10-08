#ifndef _COMPILER_H
#define _COMPILER_H

#define __must_check 	__attribute__((warn_unused_result))
#define __printf(a,b) 	__attribute__((format(printf, (a), (b))))
#define __likely(x) 	__builtin_expect(x, 1)
#define __unlikely(x) 	__builtin_expect(x, 0)

#define memcpy __builtin_memcpy
#define memcmp __builtin_memcmp
#define memset __builtin_memset
#define strlen __builtin_strlen

#define bswap_16 __builtin_bswap16
#define bswap_32 __builtin_bswap32
#define bswap_64 __builtin_bswap64

#define htons bswap_16
#define ntohs bswap_16
#define htonl bswap_32
#define ntohl bswap_32


#endif /* _COMPILER_H */
