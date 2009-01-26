/* writer.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include "writer.h"
#include "memory.h"
#include "params.h"

/*(*** writer_create */
writer *writer_create(char *data, size_t length, void *resource)
{
  writer *wr;
  
  wr = xmalloc(sizeof(writer));
  wr->wr_data = data;
  wr->wr_pos = 0;
  wr->wr_length = length;
  wr->wr_resource = resource;

  return wr;
}
/***)*/
/*(*** writer_delete */
void writer_delete(writer *wr)
{
  xfree(wr);
}
/***)*/
/*(*** writer_work */
/* Assumes the descriptor is ready and non-blocking. */
writer_result writer_work(writer *wr, int fd)
{
  int w;
  int m;

  m = wr->wr_length - wr->wr_pos;
  if(m > PARAM_MAX_WRITE) m = PARAM_MAX_WRITE;

  w = write(fd, wr->wr_data + wr->wr_pos, m);
  if(w < 0) return WRITER_ERROR;
  else {
    wr->wr_pos += w;
    if(wr->wr_pos >= wr->wr_length)
      return WRITER_DONE;
    else
      return WRITER_WORKING;
  }
}
/***)*/
