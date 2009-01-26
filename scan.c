/* scan.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <string.h>

#include "bailout.h"
#include "memory.h"
#include "scan.h"

void scan_init(scanbuf *sb, char *p, int m)
{
  sb->sb_data = p;
  sb->sb_length = m;
}

int scan_char(scanbuf *sb, int pos, char c)
{
  if(0 <= pos && pos < sb->sb_length && sb->sb_data[pos] == c) {
    return pos + 1;
  } else return SCAN_FAILURE;
}

int scan_alphabet(scanbuf *sb, int pos, alphabet alpha)
{
  if(0 <= pos && pos < sb->sb_length && alpha(sb->sb_data[pos])) {
    return pos + 1;
  } else return SCAN_FAILURE;
}

char *scan_extract(scanbuf *sb, int pos1, int pos2)
{
  if(pos1 >= 0 && pos2 >= 0 && pos2 >= pos1 &&
     pos1 < sb->sb_length && pos2 < sb->sb_length) {
    char *p;

    p = xmalloc(pos2 - pos1 + 2);
    memcpy(p, sb->sb_data + pos1, pos2 - pos1 + 1);
    p[pos2 - pos1 + 1] = 0;
    return p;
  }

  bailout("scan_extract: invalid parameters pos1=%d pos2=%d length=%d", pos1, pos2, sb->sb_length);
  return 0; /* Not reached */
}

int scan_alphabet_plus(scanbuf *sb, int pos, alphabet alpha)
{
  pos = scan_alphabet(sb, pos, alpha);
  if(pos < 0) return SCAN_FAILURE;
  while(0 <= pos && pos < sb->sb_length && alpha(sb->sb_data[pos])) pos ++;
  return pos;
}

int scan_alphabet_star(scanbuf *sb, int pos, alphabet alpha)
{
  while(0 <= pos && pos < sb->sb_length && alpha(sb->sb_data[pos])) pos ++;
  return pos;
}

int scan_nulstr(scanbuf *sb, int pos, char *u)
{
  while(*u && pos != SCAN_FAILURE) {
    pos = scan_char(sb, pos, *u);
    u ++;
  }
  return pos;
}

int scan_convert_nulstr(scanbuf *sb, int pos, int (*convert)(int), char *u)
{
  while(*u && pos != SCAN_FAILURE) {
    if(0 <= pos && pos < sb->sb_length && convert(sb->sb_data[pos]) == *u) {
      pos ++;
    } else return SCAN_FAILURE;
    u ++;
  }
  return pos;
}


void scan_upperify(char *p)
{
  while(*p) {
    *p = toupper(*p);
    p ++;
  }
}

int scan_end(scanbuf *sb, int pos)
{
  if(pos == sb->sb_length) return pos;
  else return SCAN_FAILURE;
}
