#include <test.h>

#include <kernel/strtox.h>

TEST(printf, test1) {


    char buf[28];
    ksnprintf(buf, 27, "ABC%*.*sDEF%10.*hdGHI%*.10hhd%cKLM", 10, 5, "aaa", 4, 7653, 10, 59, 5);

    
    printf("-->%s\n", buf);
    
    return TEST_FAILURE;
}


TEST(printf, test2) {
    EXPECT_FALSE(1);
    return TEST_SUCCESS;
}
