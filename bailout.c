/* bailout.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "bailout.h"

/*(*** bailout */
void bailout(char *msg, ...)
{
  va_list args;

  (void) fprintf(stderr, "PANIC: ");
  va_start(args, msg);
  (void) vfprintf(stderr, msg, args);
  va_end(args);
  (void) fprintf(stderr, "\n");
  (void) fflush(stderr);
  abort();
}
/***)*/
/*(*** bailout_with_error */
void bailout_with_error(int error, char *msg, ...)
{
  va_list args;

  (void) fprintf(stderr, "ERROR: ");
  va_start(args, msg);
  (void) vfprintf(stderr, msg, args);
  va_end(args);
  (void) fprintf(stderr, " (%s)\n", strerror(error));
  (void) fflush(stderr);
  abort();
}
/***)*/
