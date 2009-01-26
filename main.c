/* main.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>

#include "memory.h"
#include "options.h"
#include "server.h"

char *progname = 0;
static server *sv = NULL;

/*(*** main_bad_option */
void main_bad_option(char *msg, ...)
{
  va_list args;

  if(msg) {
    (void) fprintf(stderr, "Bad option: ");
    va_start(args, msg);
    (void) vfprintf(stderr, msg, args);
    va_end(args);
    (void) fprintf(stderr, "\n");
  }
  (void) fprintf(stderr, "Usage: %s %s\n", progname, options_usage);
  (void) fflush(stderr);
  exit(EXIT_FAILURE);
}
/***)*/
/*(*** main_signal_handler */
void main_signal_handler(int signal)
{
  switch(signal) {
    case SIGINT:
      if(sv) server_interrupt(sv, signal);
      break;
    default:
      break;
  }
}
/***)*/
/*(*** main */
int main(int argc, char **argv)
{
  options opt;

  progname = strrchr (*argv, '/');
  if (progname) progname ++;
  else progname = *argv;

  options_set_defaults(&opt);
  options_parse(&opt, argc, argv, main_bad_option);

  (void) signal(SIGINT, main_signal_handler); /* Discard old sigint */

  sv = server_start(&opt);
  while(server_run(sv));

  /* Avoiding race conditions with SIGINT */
  {
    server *sv2;

    sv2 = sv;
    sv = 0;
    server_shutdown(sv2);
  }

  return 0;
}
/***)*/
