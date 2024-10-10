#include "doctest/doctest.h"

#include "csv_parser.h"

#include "money.h"
#include "str.h"

struct record {
    struct string str1;
    struct string str2;
    int32_t i1;
    int64_t i2;
    money_t amount;
};

static void *get_record_ptr(void *data, int i)
{
    struct record *r = (struct record *)data;
    switch (i) {
    case 0:
        return &r->str1;
    case 1:
        return &r->str2;
    case 2:
        return &r->i1;
    case 3:
        return &r->i2;
    case 5:
        return &r->amount;
    default:
        break;
    }
    return NULL;
}

TEST_CASE("parse_line")
{
    enum column_type types[] { CT_STR, CT_STR, CT_INT32, CT_INT64, CT_IGNORE, CT_MONEY };
    struct parser_context ctx;
    init_parser(&ctx);
    ctx.cols = 6;
    ctx.types = types;
    ctx.f_get_ptr = get_record_ptr;
    SUBCASE("sep == ','")
    {
        struct record r;
        parse_line(&ctx, "   abc, def , 10, -100 ,, 123.45", &r);
        CHECK(string_cstrcmp(&r.str1, "abc") == 0);
        CHECK(string_cstrcmp(&r.str2, "def") == 0);
        CHECK(r.i1 == 10);
        CHECK(r.i2 == -100);
        CHECK(r.amount == 12345);
    }
    SUBCASE("sep == '|'")
    {
        ctx.sep = '|';
        struct record r;
        parse_line(&ctx, "   123| 4567 | -32768| 343 | sdafsfsd| 67 89.10", &r);
        CHECK(string_cstrcmp(&r.str1, "123") == 0);
        CHECK(string_cstrcmp(&r.str2, "4567") == 0);
        CHECK(r.i1 == -32768);
        CHECK(r.i2 == 343);
        CHECK(r.amount == 678910);
    }
}

struct record1 {
    int32_t i1;
    int64_t i2;
};

static void *get_record1_ptr(void *data, int i)
{
    struct record1 *r = (struct record1 *)data;
    switch (i) {
    case 0:
        return &r->i1;
    case 1:
        return &r->i2;
    default:
        break;
    }
    return NULL;
}

TEST_CASE("outputLine")
{
    column_type types[]{CT_INT32, CT_INT64};
    struct parser_context ctx;
    init_parser(&ctx);
    ctx.cols = 2;
    ctx.types = types;
    ctx.f_get_ptr = get_record1_ptr;
    struct record1 r;
    char buf[256];
    SUBCASE("sep == ','")
    {
        r.i1 = 10;
        r.i2 = -100;
        output_line(&ctx, buf, &r);
        CHECK(strcmp(buf, "10,-100") == 0);
    }

    SUBCASE("sep == '|'")
    {
        r.i1 = 10;
        r.i2 = -100;
        ctx.sep = '|';
        output_line(&ctx, buf, &r);
        CHECK(strcmp(buf, "10|-100") == 0);
    }
}
