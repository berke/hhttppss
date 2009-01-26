/* resource.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

#include "memory.h"
#include "resource.h"
#include "params.h"

char *mime_text_plain = "text/plain";
char *mime_text_html = "text/html";
char *mime_application_octet_stream = "applicaton/octet-stream";

char *guess_mime_type(char *fn, char *start, off_t length)
{
  int i;
  int m;
  int n;

  n = strlen(fn);
  if(n > 5 && !strcmp(fn + n - 5, ".html")) return mime_text_html;

  m = length;

  if(m > PARAM_SAMPLE_SIZE) m = PARAM_SAMPLE_SIZE;

  if(!length) {
    return mime_text_plain;
  } else {
    for(i = 0; i < length; i ++) {
      if(!isascii(start[i])) return mime_application_octet_stream;
    }
    return mime_text_plain;
  }
}

void resource_release(resource *rs)
{
  (void) munmap(rs->rs_start, rs->rs_length);
  xfree(rs);
}

resource *resource_obtain(char *fn)
{
  int fd;
  struct stat st;
  char *start;
  off_t length;
  resource *rs;
  
  fd = open(fn, 0644, O_RDONLY);
  if(fd < 0) {
    return 0;
  }

  if(fstat(fd, &st) < 0) goto bye;

  length = st.st_size;

  start = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
  if(start == MAP_FAILED) goto bye;

  rs = xmalloc(sizeof(resource));

  (void) close(fd);

  rs->rs_start = start;
  rs->rs_length = length;
  rs->rs_mime_type = guess_mime_type(fn, start, length);
  return rs;

bye:
  (void) close(fd);
  return 0;
}
