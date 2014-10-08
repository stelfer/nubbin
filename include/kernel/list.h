#ifndef _LIST_H
#define _LIST_H

#include <kernel.h>

struct list {
    struct list* next;
    struct list* prev;
};

#define LIST_INIT(x) { &(x), &(x) }
#define LIST_HEAD(x) struct list x = LIST_INIT(x);
static inline void INIT_LIST(struct list* list) {
    list->next = list;
    list->prev = list;
}


static inline void list_insert(struct list* item, struct list* prev, struct list* next)
{
    item->prev = prev;
    prev->next = item;
    item->next = next;
    next->prev = item;
}

static inline void list_append(struct list* item, struct list* list)
{
    list_insert(item, list->prev, list);
}

static inline void list_remove(struct list* item) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

#define list_foreach(pos, head)                \
    for(pos = (head); pos != 0; pos = pos->next == (head) ? 0 : pos->next) 

#define list_foreach_skip_first(pos, head)             \
    for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_item(pos, type, name)             \
    ((type*) ((char*)pos - (unsigned long)&((type*)0)->name))


#define list_clear(head, type, name) do {                 \
        struct list* pos;                                 \
        list_foreach_skip_first(pos, head) {                      \
            list_remove(&list_item(pos, type, name)->name); \
        }                                                   \
    } while(0)

#define list_free(head, type, name) do {          \
        struct list* pos;                                    \
        list_foreach_skip_first(pos, head) {                 \
            type* t = list_item(pos, type, name);            \
            list_remove(&t->name);                           \
            free(t);                                         \
        }                                                    \
    } while(0)


#endif /* _LIST_H */
