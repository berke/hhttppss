/* options.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#define OPT_DEFAULT_PORT 8000
#define OPT_DEFAULT_LOG_FILE "hhttppss.log"
#define OPT_DEFAULT_BACKLOG 256
#define OPT_DEFAULT_DAEMONIZE 1
#define OPT_DEFAULT_ROOT "."

typedef struct {
  int opt_port;
  char *opt_log_file;
  int opt_backlog;
  int opt_daemonize;
  int opt_max_write;
  char *opt_root;
} options;

extern char *options_usage;
void options_set_defaults(options *opt);
void options_parse(options *opt, int argc, char **argv, void (*bad_option)(char *msg, ...));

#endif
