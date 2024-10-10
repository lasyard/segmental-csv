#ifndef _SEGMENTAL_CSV_COLUMN_TYPE_H_
#define _SEGMENTAL_CSV_COLUMN_TYPE_H_

enum column_type {
    CT_STR,
    CT_CSTR,
    CT_INT32,
    CT_INT64,
    CT_BOOL,
    CT_MONEY,
    CT_DATE,
    CT_TIME,
    CT_IGNORE,
};

#ifdef __cplusplus
extern "C" {
#endif

const char *name_of(enum column_type type);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTAL_CSV_COLUMN_TYPE_H_ */
