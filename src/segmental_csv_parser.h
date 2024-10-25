#ifndef _SEGMENTAL_CSV_SEGMENTAL_CSV_PARSER_H_
#define _SEGMENTAL_CSV_SEGMENTAL_CSV_PARSER_H_

#include "csv_parser.h"
#include "segments.h"

#define MAX_LINE_LENGTH 512

struct segmental_parser_context {
    struct parser_context item_parser_context;
    struct parser_context segment_parser_context;
    size_t item_data_size;
    size_t segment_data_size;
};

#ifdef __cplusplus
extern "C" {
#endif

struct item *add_new_item(const struct segmental_parser_context *ctx, struct segment *segment);

struct segment *add_new_segment(const struct segmental_parser_context *ctx, struct segments *segments);

int segmental_parse(
    const struct segmental_parser_context *ctx,
    struct segments *segments,
    size_t (*read)(void *context, char *buf, size_t len),
    void *context
);

int segmental_output(
    const struct segmental_parser_context *ctx,
    struct segments *segments,
    void (*write)(void *context, const char *buf, size_t len),
    void *context
);

void release_segments(const struct segmental_parser_context *ctx, struct segments *segments);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTAL_CSV_SEGMENTAL_CSV_PARSER_H_ */
