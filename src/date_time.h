#ifndef _SEGMENTAL_CSV_DATE_TIME_H_
#define _SEGMENTAL_CSV_DATE_TIME_H_

#include <stdint.h>

typedef int32_t date_t;
typedef int32_t dtime_t;

#ifdef __cplusplus
extern "C" {
#endif

int jdn(int year, int month, int day);
void jdn_split(int jdn, int *year, int *month, int *day);

const char *parse_date(const char *buf, date_t *data, char sep, char date_sep);
const char *parse_time(const char *buf, dtime_t *data, char sep);

char *output_date(char *buf, date_t data, char dateSep);
char *output_time(char *buf, dtime_t data);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTAL_CSV_DATE_TIME_H_ */
