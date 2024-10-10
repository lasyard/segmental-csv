#include "column_type.h"

const char *name_of(enum column_type type)
{
    switch (type) {
    case CT_STR:
        return "STR";
    case CT_CSTR:
        return "CSTR";
    case CT_INT32:
        return "INT32";
    case CT_INT64:
        return "INT64";
    case CT_BOOL:
        return "BOOL";
    case CT_MONEY:
        return "MONEY";
    case CT_DATE:
        return "DATE";
    case CT_TIME:
        return "TIME";
    case CT_IGNORE:
        break;
    }
    return "IGNORE";
}
