#ifndef _TEST_H
#define _TEST_H

#define kmalloc malloc
#define kfree free

#include <kernel.h>
#include <kernel/list.h>
#include <colors.h>

int printf(const char* format, ...);
int puts(const char* s);

typedef int (*__test)(void);
struct test {
    struct list list;
    __test test;
    const char* name;
    const char* module;
};

static struct test first_test = { LIST_INIT(first_test.list), 0, 0, 0};

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

#define TEST_START_MSG(mod, name)   printf(GREEN("[ RUN      ]") " %s.%s\n", mod, name)
#define TEST_SUCCESS_MSG(mod, name) printf(GREEN("[       OK ]") " %s.%s\n", mod, name)
#define TEST_FAILURE_MSG(mod, name) printf(  RED("[     FAIL ]") " %s.%s\n", mod, name)

#define PASSED_MSG() printf(GREEN("[  PASSED  ]") "\n")
#define FAILED_MSG() printf(  RED("[  FAILED  ]") "\n")


#define LOG_FAILURE(fmt, ...) do {              \
        printf("%s:%d: Failure\n Condition: ", __FILE__, __LINE__);  \
        printf(fmt, __VA_ARGS__);                        \
        puts("");                                     \
    } while(0)

#define EXPECT_TRUE(x) do {                     \
        if (!(x)) {                             \
            LOG_FAILURE("(%s) is false", STRINGIFY(x));    \
            return TEST_FAILURE;                \
        }                                       \
    } while (0)

#define EXPECT_FALSE(x) do {                     \
        if ((x)) {                             \
            LOG_FAILURE("(%s) is true", STRINGIFY(x));    \
            return TEST_FAILURE;                \
        }                                       \
    } while (0)

#define EXPECT_EQ(x,y) do {                                             \
        if ((x) != (y)) {                                               \
            LOG_FAILURE("(%s) != (%s)", STRINGIFY(x), STRINGIFY(y));    \
            return TEST_FAILURE;                                        \
        }                                                               \
    } while (0)

#define EXPECT_STREQ(x, y, n) do {                                      \
        if (memcmp((x), (y), (n))) {                                    \
            LOG_FAILURE("(%s) != (%s)", STRINGIFY(x), STRINGIFY(y));    \
            return TEST_FAILURE;                                        \
        }                                                               \
    } while(0)

#define TEST(mod, x)                                                      \
    struct test TEST_struct_##x = {LIST_INIT(TEST_struct_##x.list), 0, 0}; \
    int TEST_impl_##x();                                                \
    void TEST_##x() __attribute__((constructor));                       \
    void TEST_##x() {                                                   \
        TEST_struct_##x.name = STRINGIFY(x);                                 \
        TEST_struct_##x.module = STRINGIFY(mod);                     \
        TEST_struct_##x.test = TEST_impl_##x;                           \
        list_append(&TEST_struct_##x.list, &first_test.list);   \
    }                                                                   \
    int TEST_impl_##x() 


inline int main() {
    struct test* p;
    int status = TEST_SUCCESS;
    struct list* l;
    list_foreach_skip_first(l, &first_test.list) {
        p = list_item(l, struct test, list);
        TEST_START_MSG(p->module, p->name);
        int q = p->test();
        if (q == TEST_FAILURE) {
            status = q;
            TEST_FAILURE_MSG(p->module, p->name);
        } else {
            TEST_SUCCESS_MSG(p->module, p->name);
        }
    }
    if (status == TEST_SUCCESS) {
        PASSED_MSG();
    } else {
        FAILED_MSG();
    }
    return status;
}

#endif /* _TEST_H */
