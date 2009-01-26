/* memory.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

void *xmalloc(size_t size);
void *xrealloc(void *, size_t size);
int xasprintf(char **strp, const char *fmt, ...);
void xfree(void *);
#endif
