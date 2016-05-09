#include <test.h>

struct foo
{
    struct list list;
    int a;
};

TEST(list, create_update_free) {
    struct foo f = { LIST_INIT(f.list), 0};

    int i;
    for(i=0; i < 10; ++i) {
        struct foo* f2 = kmalloc(sizeof(struct foo));
        list_append(&f2->list, &f.list);
        f2->a = i + 1;
    }

    struct list* pos;
    
    list_foreach(pos, &f.list) {
        struct foo* f = list_item(pos, struct foo, list);
    }

    list_foreach_skip_first(pos, &f.list) {
        struct foo* f = list_item(pos, struct foo, list);
        list_remove(&f->list);
        kfree(f);
    }

    list_foreach(pos, &f.list) {
        struct foo* f = list_item(pos, struct foo, list);
    }
    
    return TEST_SUCCESS;
}

TEST(list, list_free) {
    struct foo f = { LIST_INIT(f.list), 0};
    int i;
    for(i=0; i < 10; ++i) {
        struct foo* f2 = kmalloc(sizeof(struct foo));
        list_append(&f2->list, &f.list);
        f2->a = i + 1;
    }

    list_free(&f.list, struct foo, list);
    struct list* pos;
    list_foreach(pos, &f.list) {
        struct foo* f = list_item(pos, struct foo, list);
    }
    return TEST_SUCCESS;
}
