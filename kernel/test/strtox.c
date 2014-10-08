
#include <test.h>
#include <kernel/strtox.h>

#define STRTOX_TEST_U(p, b, rc, c) do {           \
        unsigned long r;                          \
        int rv = strtoul(p, b, &r);               \
        EXPECT_TRUE(rv > 0);                      \
        EXPECT_EQ(rc, r);                         \
        EXPECT_EQ(strlen(c), strlen(p) - rv);     \
    } while(0)

#define STRTOX_TEST_S(p, b, rc, c) do {   \
        long r;                                   \
        int rv = strtol(p, b, &r);               \
        EXPECT_TRUE(rv > 0);                      \
        EXPECT_EQ(rc, r);                         \
        EXPECT_EQ(strlen(c), strlen(p) - rv);     \
    } while(0)

TEST(strtox, U_SUCCESS) {
    STRTOX_TEST_U("0x10", 16, 16, "");
    STRTOX_TEST_U("0x10.1234", 16, 16, ".1234");
    STRTOX_TEST_U("10.1234", 10, 10, ".1234");
    STRTOX_TEST_U("+10ABCD.1234", 10, 10, "ABCD.1234");
    return TEST_SUCCESS;
}

TEST(strtox, S_SUCCESS) {
    STRTOX_TEST_S("0x10", 16, 16, "");
    STRTOX_TEST_S("0x10.1234", 16, 16, ".1234");
    STRTOX_TEST_S("10.1234", 10, 10, ".1234");
    STRTOX_TEST_S("+10ABCD.1234", 10, 10, "ABCD.1234");
    STRTOX_TEST_S("-0x10", 16, -16, "");
    STRTOX_TEST_S("-0x10.1234", 16, -16, ".1234");
    STRTOX_TEST_S("-10.1234", 10, -10, ".1234");
    STRTOX_TEST_S("-10ABCD.1234", 10, -10, "ABCD.1234");
    return TEST_SUCCESS;
}

#define STRTOX_LIMIT_TEST_U(p, b, c) STRTOX_TEST_U(STRINGIFY(p), b, p, c)
TEST(strtox, U_LIMITS) {
    STRTOX_LIMIT_TEST_U(255, 10, "");
    STRTOX_LIMIT_TEST_U(65535, 10, "");
    STRTOX_LIMIT_TEST_U(4294967295UL, 10, "UL");
    STRTOX_LIMIT_TEST_U(18446744073709551615ULL, 10, "ULL");
    STRTOX_LIMIT_TEST_U(0xff, 16, "");
    STRTOX_LIMIT_TEST_U(0xffff, 16, "");
    STRTOX_LIMIT_TEST_U(0xffffffffff, 16, "");
    STRTOX_LIMIT_TEST_U(0xffffffffffffffffUL, 16, "UL");
    return TEST_SUCCESS;
}

#define STRTOX_LIMIT_TEST_S(p, b, c) STRTOX_TEST_S(STRINGIFY(p), b, p, c)
TEST(strtox, S_LIMITS) {
    STRTOX_LIMIT_TEST_S(-128, 10, "");
    STRTOX_LIMIT_TEST_S(-32767, 10, "");
    STRTOX_LIMIT_TEST_S(-2147483647, 10, "");
    STRTOX_LIMIT_TEST_S(-9223372036854775807LL, 10, "LL");
    STRTOX_LIMIT_TEST_S(-0x7f, 16, "");
    STRTOX_LIMIT_TEST_S(-0x7fff, 16, "");
    STRTOX_LIMIT_TEST_S(-0x7fffffffff, 16, "");
    STRTOX_LIMIT_TEST_S(-0x7fffffffffffffffUL, 16, "UL");
    return TEST_SUCCESS;
}
