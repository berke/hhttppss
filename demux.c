/* demux.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include "demux.h"
#include "params.h"

/*(*** demux_create */
demux *demux_create(void *data)
{
  demux *dx;

  dx = xmalloc(sizeof(demux));
  dx->dx_count = 0;
  dx->dx_size = PARAM_AVERAGE_FDS;
  dx->dx_pollfds = xmalloc(sizeof(struct pollfd) * dx->dx_size);
  dx->dx_connections = intmap_create(dx->dx_size);
  dx->dx_data = data;
  dx->dx_deletion_stack = stack_create();

  return dx;
}
/***)*/
/*(*** demux_shutdown */
void demux_shutdown(demux *dx)
{
  int i;
  int fd;
  connection *cn;

  xfree(dx->dx_pollfds);
  for(i = 0; i<dx->dx_count; i++) {
    fd = dx->dx_pollfds->fd;
    cn = intmap_get(dx->dx_connections, fd);
    xfree(cn); /* Shutdown connection */
  }
  xfree(dx->dx_pollfds);
  intmap_shutdown(dx->dx_connections);
  xfree(dx);
}
/***)*/
/*(*** demux_set_event_mask */
void demux_set_event_mask(demux *dx, connection *cn, short events)
{
  struct pollfd *p;
  p = dx->dx_pollfds + cn->cn_pollfd_i;
  p->events = events;
}
/***)*/
/*(*** demux_set_data */
void demux_set_data(demux *dx, void *data)
{
  dx->dx_data = data;
}
/***)*/
/*(*** demux_set_connection_data */
void demux_set_connection_data(connection *cn, void *data)
{
  cn->cn_data = data;
}
/***)*/
/*(*** demux_delete_connection */
void demux_delete_connection(demux *dx, connection *cn)
{
  if(!cn->cn_marked_for_deletion) {
    cn->cn_marked_for_deletion = 1;
    stack_push(dx->dx_deletion_stack, cn);
  }
}
/***)*/
/*(*** demux_really_delete_connection */
static void demux_really_delete_connection(demux *dx, connection *cn)
{
  int i, m;

  /* Call shutdown method */
  cn->cn_delete(dx->dx_data, cn);

  /* Close FD */
  (void) close(cn->cn_fd);

  /* Remove from tables */
  i = cn->cn_pollfd_i;
  m = dx->dx_count;
  if(i < m - 1) dx->dx_pollfds[i] = dx->dx_pollfds[m - 1]; /* Copy last entry */
  dx->dx_count --;
  intmap_set(dx->dx_connections, cn->cn_fd, 0);
  xfree(cn);
}
/***)*/
/*(*** demux_add_connection */
connection *
demux_add_connection(
    demux *dx,
    int fd, 
    short events,
    connection_handle hd, 
    connection_delete dl,
    void *data)
{
  connection *cn;
  cn = xmalloc(sizeof(connection));
  cn->cn_fd = fd;
  cn->cn_handle = hd;
  cn->cn_delete = dl;
  cn->cn_data = data;
  cn->cn_marked_for_deletion = 0;
  intmap_set(dx->dx_connections, fd, cn);

  /* Adjust pollfd table */
  int i;
  if(dx->dx_count > dx->dx_size) {
    /* We'll need to resize the table. */
    dx->dx_size <<= 1;
    dx->dx_pollfds = xrealloc(dx->dx_pollfds, sizeof(struct pollfd) * dx->dx_size);
  }
  i = dx->dx_count;
  dx->dx_count = i + 1;

  cn->cn_pollfd_i = i;

  /* Add pollfd entry */
  dx->dx_pollfds[i].fd = fd;
  dx->dx_pollfds[i].events = events;

  return cn;
}
/***)*/
/*(*** demux_poll_and_run */
void demux_poll_and_run(demux *dx, log *lg)
{
  int r;

  /* Got any connections to delete ? */
  while(!stack_is_empty(dx->dx_deletion_stack)) {
    connection *cn;

    cn = stack_pop(dx->dx_deletion_stack);
    demux_really_delete_connection(dx, cn);
  }

  /* Poll IO */
  r = poll(dx->dx_pollfds, dx->dx_count, PARAM_POLL_TIMEOUT);
  if(r < 0) {
    if(errno == EINTR) return;
    log_msg(lg, LOG_ERROR, "Error in poll: %s", strerror(errno));
    /* Nothing much we can do about this... */
  } else if(r > 0) {
    int fd;
    int i;
    int x;
    struct pollfd *p;
    connection *cn;
    int m;

    m = dx->dx_count;

    for(i = 0; i < m; i ++) {
      p = dx->dx_pollfds + i;
      fd = p->fd;
      x = p->revents;

      if(x) {
        cn = intmap_get(dx->dx_connections, fd);
        if(!cn)
          /* Must have been deleted. */
          continue;
        if(x & POLLERR) {
          log_msg(lg, LOG_ERROR, "Error on FD %d", p->fd);
        }
        if(cn->cn_handle) {
          cn->cn_handle(dx->dx_data, cn, x);
        }
      }
    }
  } else {
    log_msg(lg, LOG_DEBUG, "Tick");
  }
}
/***)*/
