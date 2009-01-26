/* log.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

#include "bailout.h"
#include "memory.h"
#include "options.h"
#include "log.h"

static char *log_priority_strings[] = {
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
};

/*(*** log_init */
log *log_init(options *opt)
{
  log *l;

  l = xmalloc(sizeof(log));
  l->log_out = fopen(opt->opt_log_file, "a");
  if(!l->log_out) bailout("Cannot open log file %s for appending", opt->opt_log_file);
  return l;
}
/***)*/
/*(*** log_shutdown */
void log_shutdown(log *l)
{
  (void) fclose(l->log_out);
  xfree(l);
}
/***)*/
/*(*** log_msg */
void log_msg(log *l, log_priority p, char *msg, ...)
{
  FILE *f;
  va_list args;
  char *ps;
  time_t t;
  struct tm tm;

  return;

  if(0 <= p && p < LOG_COUNT)
    ps = log_priority_strings[p];
  else
    ps = "UNKNOWN";

  f = l->log_out;

  t = time(0);
  (void) localtime_r(&t, &tm);
  (void) fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d %s: ",
                    tm.tm_year + 1900,
                    tm.tm_mon + 1,
                    tm.tm_mday,
                    tm.tm_hour,
                    tm.tm_min,
                    tm.tm_sec,
                    ps);
  va_start(args, msg);
  (void) vfprintf(f, msg, args);
  va_end(args);
  (void) fprintf(f, "\n");
  (void) fflush(f);
}
/***)*/
