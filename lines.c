/* lines.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

/*
 * Converts a byte stream to a string stream
 * strings being delimited by CR LF pairs
 */

#include <string.h>

#include "memory.h"
#include "lines.h"
#include "params.h"

/*(*** lines_create */
lines *lines_create(void (*eater)(void *data, char *p, int m), void *data)
{
  lines *ls;

  ls = xmalloc(sizeof(lines));
  ls->ls_size = PARAM_AVERAGE_LINE_LENGTH;
  ls->ls_buffer = xmalloc(ls->ls_size);
  ls->ls_length = 0;
  ls->ls_line_eater = eater;
  ls->ls_state = LINES_INIT;
  ls->ls_data = data;
  ls->ls_limit = PARAM_REASONABLE_LINE_LIMIT;

  return ls;
}
/***)*/
/*(*** lines_delete */
void lines_delete(lines *ls)
{
  xfree(ls->ls_buffer);
  xfree(ls);
}
/***)*/
/*(*** lines_add_char */
static inline void lines_add_char(lines *ls, char c)
{
  if(ls->ls_length >= ls->ls_size) {
    /* We need to grow the buffer... */
    ls->ls_size <<= 1;
    ls->ls_buffer = xrealloc(ls->ls_buffer, ls->ls_size);
  }
  ls->ls_buffer[ls->ls_length ++] = c;
}
/***)*/
/*(*** lines_add_bytes */
void lines_add_bytes(lines *ls, char *p, int m)
{
  int i;

  for(i = 0; i < m; i ++) {
    char c;

    c = p[i];

    if(ls->ls_limit && ls->ls_length > ls->ls_limit) goto eat;

    switch(ls->ls_state) {
      case LINES_INIT:
        switch(c) {
          case '\r':
            ls->ls_state = LINES_CR;
            break;
          default:
            lines_add_char(ls, c);
            break;
        }
        break;
      case LINES_CR:
        switch(c) {
          case '\n':
            /* That's it, we've got a CR LF. */
          eat:
            if(ls->ls_line_eater) {
              ls->ls_line_eater(ls->ls_data, ls->ls_buffer, ls->ls_length);
            }
            ls->ls_state = LINES_INIT;
            ls->ls_length = 0;
            break;
          case '\r':
            ls->ls_state = LINES_CR;
            break;
          default:
            /* Was expecting a LF. Assume it is a standard character. */
            lines_add_char(ls, '\r');
            lines_add_char(ls, c);
            ls->ls_state = LINES_INIT;
            break;
        }
        break;
    }
  }
}
/***)*/
/*(*** lines_contents */
char *lines_contents(lines *ls)
{
  char *p;
  
  p = xmalloc(ls->ls_length + 1);
  memcpy(p, ls->ls_buffer, ls->ls_length);
  p[ls->ls_length] = 0;
  return p;
}
/***)*/
