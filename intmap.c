/* intmap.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <assert.h>
#include "bailout.h"
#include "memory.h"
#include "intmap.h"

/*(*** intmap_create */
intmap *intmap_create(int size)
{
  int i;
  intmap *im;

  assert(size > 0);

  im = xmalloc(sizeof(intmap));
  im->im_size = size;
  im->im_table = xmalloc(sizeof(void *) * size);

  for(i = 0; i < size; i ++) {
    im->im_table[i] = 0;
  }
  return im;
}
/***)*/
/*(*** intmap_shutdown */
void intmap_shutdown(intmap *im)
{
  xfree(im->im_table);
  xfree(im);
}
/***)*/
/*(*** intmap_size */
int intmap_size(intmap *im)
{
  return im->im_size;
}
/***)*/
/*(*** intmap_get */
void *intmap_get(intmap *im, int i)
{
  assert(i >= 0);
  if(i < im->im_size) return im->im_table[i];
  else return 0;
}
/***)*/
/*(*** intmap_nextsize */
int intmap_nextsize(int size, int index)
{
  if(!size) size = 1;
  while(index >= size) {
    size <<= 1;
    if(size < 0) bailout("intmap_nextsize: Integer overflow");
  }
  return size;
}
/*)***/
/*(*** intmap_resize */
void intmap_resize(intmap *im, int index)
{
  int size;
  assert(index >= 0);
  if(index >= im->im_size) {
    size = intmap_nextsize(im->im_size, index);
    im->im_table = xrealloc(im->im_table, sizeof(void *) * size);
  }
}
/***)*/
/*(*** intmap_set */
void intmap_set(intmap *im, int i, void *x)
{
  assert(i >= 0);
  intmap_resize(im, i);
  im->im_table[i] = x;
}
/***)*/
