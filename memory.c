/* memory.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <stdio.h>
#include <stdarg.h>

#include "memory.h"
#include "bailout.h"

/*(*** xmalloc */
void *xmalloc(size_t size)
{
  void *p;
  
  if(size <= 0) bailout("xmalloc: size=%ld <= 0", size);
  p = malloc(size);
  if(!p) bailout("xmalloc: failed with size=%ld", size);
  return p;
}
/***)*/
/*(*** xasprintf */
int xasprintf(char **strp, const char *fmt, ...)
{
  int m;
  va_list args;

  va_start(args, fmt);
  m = vasprintf(strp, fmt, args);
  va_end(args);
  if(m < 0) bailout("xvasprintf: failed with format %s", fmt);
  return m;
}
/***)*/
/*(*** xrealloc */
void *xrealloc(void *p, size_t size)
{
  if(size <= 0) bailout("xrealloc: size=%ld <= 0", size);
  p = realloc(p, size);
  if(!p) bailout("xrealloc: failed with size=%ld", size);
  return p;
}
/***)*/
/*(*** xfree */
void xfree(void *p)
{
  if(!p) bailout("xfree: attempt to free null pointer");
  free(p);
}
/***)*/
