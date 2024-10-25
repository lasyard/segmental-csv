#ifndef _SEGMENTAL_CSV_CSV_PARSER_H_
#define _SEGMENTAL_CSV_CSV_PARSER_H_

#include "column_type.h"

struct parser_context {
    int cols;                              // The number of columns.
    const enum column_type *types;         // The types of each column.
    char sep;                              // The separator of fields.
    char num_sep;                          // The separator in numbers.
    char date_sep;                         // The separator of y/m/d in date.
    int money_prec;                        // The precision of money.
    int money_scale;                       // The scale factor of money.
    void *(*f_get_ptr)(void *data, int i); // Function to get member ptr of data.
};

#ifdef __cplusplus
extern "C" {
#endif

void init_parser(struct parser_context *ctx);

void set_money_prec(struct parser_context *ctx, int money_prec);

void init_data(const struct parser_context *ctx, void *data);

const char *parse_line(const struct parser_context *ctx, const char *line, void *data);

char *output_line(const struct parser_context *ctx, char *buf, const void *data);

void release_data(const struct parser_context *ctx, void *data);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTAL_CSV_CSV_PARSER_H_ */
