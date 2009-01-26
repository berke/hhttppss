/* bailout.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef BAILOUT_H
#define BAILOUT_H

void bailout(char *msg, ...);
void bailout_with_error(int error, char *msg, ...);

#endif
