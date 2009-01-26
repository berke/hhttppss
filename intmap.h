/* intmap.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef INTMAP_H
#define INTMAP_H

typedef struct {
  int im_size;
  void **im_table;
} intmap;

intmap *intmap_create(int size);
void intmap_shutdown(intmap *im);
void *intmap_get(intmap *im, int i);
int intmap_size(intmap *im);
void intmap_resize(intmap *im, int size);
void intmap_set(intmap *im, int i, void *x);

#endif
