#include <test.h>

#include <kernel/snprintf.h>
#include <kernel/strtox.h>


#define TEST_FMT(a, b) do {                     \
        struct conv_spec s = b;                 \
} while(0)


TEST(printf, t11) {
    
    TEST_FMT(1, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
    char buf[28];
    struct conv_spec s;
    snprintf_spec(&s, buf, 27, "%*.*s", 10, 5, "abc");
    return TEST_FAILURE;
}


TEST(printf, test1) {
    /* char buf[28]; */
    /* ksnprintf(buf, 27, "ABC%*.*sDEF%10.*hdGHI%*.10hhd%cKLM", 10, 5, "aaa", 42, 7653, 10, 59, 5); */

    
    /* printf("-->%s\n", buf); */
    
    return TEST_FAILURE;
}


TEST(printf, test2) {
    EXPECT_FALSE(1);
    return TEST_SUCCESS;
}
