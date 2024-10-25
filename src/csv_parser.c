#include "csv_parser.h"

#include <stdlib.h>

#include "date_time.h"
#include "defs.h"
#include "int.h"
#include "money.h"
#include "str.h"

static void init_by_type(enum column_type type, void *data)
{
    switch (type) {
    case CT_STR: {
        struct string *string = (struct string *)data;
        string->buf = NULL;
        string->len = 0;
        break;
    }
    case CT_CSTR:
        *(char **)data = NULL;
        break;
    case CT_INT32:
        *(int32_t *)data = 0;
        break;
    case CT_INT64:
        *(int64_t *)data = 0LL;
        break;
    case CT_BOOL:
        *(bool *)data = false;
        break;
    case CT_MONEY:
        *(money_t *)data = 0LL;
        break;
    case CT_DATE:
        *(date_t *)data = 0;
        break;
    case CT_TIME:
        *(dtime_t *)data = 0;
        break;
    default:
        break;
    }
}

static const char *parse_by_type(const struct parser_context *ctx, const char *buf, enum column_type type, void *data)
{
    switch (type) {
    case CT_STR:
        return parse_string(buf, (struct string *)data, ctx->sep);
    case CT_CSTR:
        return parse_cstring(buf, (char **)data, ctx->sep);
    case CT_INT32:
        return parse_int32(buf, (int32_t *)data, ctx->sep);
    case CT_INT64:
        return parse_int64(buf, (int64_t *)data, ctx->sep);
    case CT_BOOL:
        return parse_bool(buf, (bool *)data, ctx->sep);
    case CT_MONEY:
        return parse_money(buf, (money_t *)data, ctx->sep, ctx->money_scale, ctx->num_sep);
    case CT_DATE:
        return parse_date(buf, (date_t *)data, ctx->sep, ctx->date_sep);
    case CT_TIME:
        return parse_time(buf, (dtime_t *)data, ctx->sep);
    case CT_IGNORE: {
        const char *p;
        for (p = buf; *p != ctx->sep && !is_line_end(*p); ++p) {
        }
        return p;
    }
    }
    return NULL;
}

static char *output_by_type(const struct parser_context *ctx, char *buf, enum column_type type, const void *data)
{
    switch (type) {
    case CT_STR:
        return output_string(buf, (const struct string *)data);
    case CT_CSTR:
        return output_cstring(buf, *(const char *const *)data);
    case CT_INT32:
        return output_int32(buf, *(const int32_t *)data);
    case CT_INT64:
        return output_int64(buf, *(const int64_t *)data);
    case CT_BOOL:
        return output_bool(buf, *(const bool *)data);
    case CT_MONEY:
        return output_money(buf, *(const money_t *)data, ctx->money_prec, ctx->money_scale);
    case CT_DATE:
        return output_date(buf, *(const date_t *)data, ctx->date_sep);
    case CT_TIME:
        return output_time(buf, *(const dtime_t *)data);
    case CT_IGNORE:
        break;
    }
    return buf;
}

void init_parser(struct parser_context *ctx)
{
    ctx->cols = 0;
    ctx->types = NULL;
    ctx->sep = ',';
    ctx->num_sep = ' ';
    ctx->date_sep = '-';
    set_money_prec(ctx, 2);
    ctx->f_get_ptr = NULL;
}

void set_money_prec(struct parser_context *ctx, int money_prec)
{
    ctx->money_prec = money_prec;
    ctx->money_scale = 1;
    for (int i = 0; i < money_prec; ++i) {
        ctx->money_scale *= 10;
    }
}

void init_data(const struct parser_context *ctx, void *data)
{
    for (int i = 0; i < ctx->cols; ++i) {
        enum column_type type = ctx->types[i];
        init_by_type(type, ctx->f_get_ptr(data, i));
    }
}

const char *parse_line(const struct parser_context *ctx, const char *line, void *data)
{
    const char *p = line;
    for (int i = 0; i < ctx->cols; ++i) {
        enum column_type type = ctx->types[i];
        const char *b = p;
        p = parse_by_type(ctx, b, type, ctx->f_get_ptr(data, i));
        return_null_if_null(p);
        ++p; // Skip the sep
    }
    return p;
}

char *output_line(const struct parser_context *ctx, char *buf, const void *data)
{
    char *p = buf;
    for (int i = 0; i < ctx->cols; ++i) {
        p = output_by_type(ctx, p, ctx->types[i], (const void *)(ctx->f_get_ptr((void *)data, i)));
        if (i < ctx->cols - 1) {
            *(p++) = ctx->sep;
        }
    }
    *(p++) = '\n';
    return p;
}

void release_data(const struct parser_context *ctx, void *data)
{
    for (int i = 0; i < ctx->cols; ++i) {
        enum column_type type = ctx->types[i];
        if (type == CT_CSTR) {
            char **cstr = (char **)(ctx->f_get_ptr(data, i));
            free(*cstr);
        }
    }
}
