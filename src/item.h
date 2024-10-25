#ifndef _SEGMENTAL_CSV_ITEM_H_
#define _SEGMENTAL_CSV_ITEM_H_

#include "list.h"

struct item {
    struct list_item list;
    void *data;
};

#define get_item(ptr) list_entry(ptr, struct item, list)

#ifdef __cplusplus
extern "C" {
#endif

void init_item(struct item *item, void *data);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTAL_CSV_ITEM_H_ */
