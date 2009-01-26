/* demux.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef DEMUX_H
#define DEMUX_H

#include <poll.h>

#include "log.h"
#include "memory.h"
#include "intmap.h"
#include "stack.h"

struct connection_struct;

typedef void (*connection_handle)(void *, struct connection_struct *, short events);
typedef void (*connection_delete)(void *, struct connection_struct *);

typedef struct connection_struct {
  int cn_fd; /* File descriptor for this connection */
  int cn_pollfd_i; /* Current position in pollfd table */
  connection_handle cn_handle; /* Methods */
  connection_delete cn_delete;
  void *cn_data; /* User data... aka DIY closures */
  int cn_marked_for_deletion; /* Close this connection as soon as appropriate */
} connection;

typedef struct {
  int dx_count; /* Entries [0..dx_count-1] of the pollfds table are used */
  int dx_size; /* Size of the pollfds table */
  struct pollfd *dx_pollfds; /* The actual table */
  intmap *dx_connections; /* Another table for mapping fds to connections. */
  void *dx_data; /* User data for connection methods (first argument) */
  stack *dx_deletion_stack; /* Connections marked for deletion */
} demux;

demux *demux_create(void *);
void demux_shutdown(demux *dx);
void demux_delete_connection(demux *dx, connection *cn);
void demux_set_event_mask(demux *dx, connection *cn, short events);
connection *
demux_add_connection(
    demux *dx,
    int fd, 
    short events,
    connection_handle rd, 
    connection_delete sh,
    void *data);
void demux_set_data(demux *dx, void *data);
void demux_set_connection_data(connection *cn, void *data);
void demux_poll_and_run(demux *dx, log *lg);
#endif
