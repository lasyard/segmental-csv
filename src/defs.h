#ifndef _SEGMENTAL_CSV_DEFS_H_
#define _SEGMENTAL_CSV_DEFS_H_

#include <stdbool.h>
#include <stddef.h>

#define return_null_if_null(ptr) \
    do {                         \
        if ((ptr) == NULL) {     \
            return NULL;         \
        }                        \
    } while (false)

#endif /* _SEGMENTAL_CSV_DEFS_H_ */
