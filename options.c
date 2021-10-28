/* options.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include "options.h"

#include <getopt.h>
#include <stdlib.h>

char *options_usage = "[-d][-b <backlog>][-p <port>][-D <rootdir>]";

static char *option_string = "b:dp:D:";

/*(*** options_parse */
void options_parse(options *opt, int argc, char **argv, void (*bad_option)(char *msg, ...))
{
  int c;
  int x;

  for(;;) {
    c = getopt(argc, argv, option_string);
    if(c < 0) break;
    switch(c) {
      case 'b': /* backlog */
        x = atoi(optarg);
        if(x < 0) bad_option("Backlog value %d negative", x);
        opt->opt_backlog = x;
        break;
      case 'd': /* don't daemonize */
        opt->opt_daemonize = 0;
        break;
      case 'p': /* port */
        x = atoi(optarg);
        if(x < 1 || x > 65535) bad_option("Port value %d out of range", x);
        opt->opt_port = x;
        break;
      case 'D': /* root directory */
	opt->opt_root = optarg;
	break;
      case '?':
      default:
        bad_option(NULL);
        break;
    }
  }
}
/***)*/
/*(*** options_set_defaults */
void options_set_defaults(options *opt)
{
  opt->opt_log_file = OPT_DEFAULT_LOG_FILE;
  opt->opt_port = OPT_DEFAULT_PORT;
  opt->opt_backlog = OPT_DEFAULT_BACKLOG;
  opt->opt_daemonize = OPT_DEFAULT_DAEMONIZE;
  opt->opt_root = OPT_DEFAULT_ROOT;
}
/***)*/
