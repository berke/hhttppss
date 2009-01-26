/* log.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "options.h"

typedef enum {
  LOG_DEBUG, /* 0 */
  LOG_INFO,  /* 1 */
  LOG_WARN,  /* 2 */
  LOG_ERROR, /* 3 */
  LOG_COUNT
} log_priority;

typedef struct {
  FILE *log_out;
} log;

log *log_init(options *opt);
void log_shutdown(log *l);
void log_msg(log *l, log_priority p, char *msg, ...);

#endif
