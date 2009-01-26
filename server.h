/* server.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef SERVER_H
#define SERVER_H

#include "options.h"
#include "log.h"
#include "demux.h"

typedef struct {
  log *sv_log;
  int sv_running;
  int sv_interrupting_signal;
  unsigned long long sv_cycles;
  int sv_socket;
  demux *sv_demux;
} server;

server *server_start(options *opt);
void server_shutdown(server *sv);
int server_run(server *sv);
void server_interrupt(server *sv, int signal);

#endif
