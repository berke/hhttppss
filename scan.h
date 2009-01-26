/* scan.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef SCAN_H
#define SCAN_H

#include <ctype.h>

#define SCAN_FAILURE (-1)

typedef struct {
  char *sb_data;
  int sb_length;
} scanbuf;

typedef int (*alphabet)(int c);

void scan_init(scanbuf *sb, char *p, int m);
int scan_char(scanbuf *sb, int pos, char c);
int scan_alphabet(scanbuf *sb, int pos, alphabet a);
int scan_alphabet_star(scanbuf *sb, int pos, alphabet a);
int scan_alphabet_plus(scanbuf *sb, int pos, alphabet a);
int scan_nulstr(scanbuf *sb, int pos, char *u);
int scan_convert_nulstr(scanbuf *sb, int pos, int (*convert)(int), char *u);
int scan_end(scanbuf *sb, int pos);
char *scan_extract(scanbuf *sb, int pos1, int pos2);
void scan_upperify(char *p);

#endif
