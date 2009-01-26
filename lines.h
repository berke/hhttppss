/* lines.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef LINES_H
#define LINES_H

typedef enum {
  LINES_INIT, /* Started processing a line */
  LINES_CR,   /* Saw a CR */
} lines_state;

typedef struct {
  char *ls_buffer; /* Buffer */
  int ls_size;     /* Size of the buffer */
  int ls_length;   /* Length of the line */
  lines_state ls_state; /* Automaton state */
  int ls_limit;    /* Maximum length of lines */
  void *ls_data;   /* User data */
  void (*ls_line_eater)(void *data, char *p, int m);
} lines;

lines *lines_create(void (*eater)(void *data, char *p, int m), void *data);
void lines_delete(lines *ls);
void lines_add_bytes(lines *ls, char *p, int m);
char *lines_contents(lines *ls);

#endif
