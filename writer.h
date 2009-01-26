/* writer.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef WRITER_H
#define WRITER_H

#include <unistd.h>

typedef enum {
  WRITER_ERROR,
  WRITER_DONE,
  WRITER_WORKING
} writer_result;

typedef struct {
  char *wr_data;
  size_t wr_pos;
  size_t wr_length;
  void *wr_resource;
} writer;

writer *writer_create(char *data, size_t length, void *resource);
void writer_delete(writer *wr);
writer_result writer_work(writer *wr, int fd);

#endif
