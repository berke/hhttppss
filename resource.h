/* resource.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include <sys/types.h>

typedef struct {
  char *rs_start;
  off_t rs_length;
  char *rs_mime_type;
} resource;

resource *resource_obtain(char *fn);
void resource_release(resource *rs);

#endif
