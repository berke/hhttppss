/* server.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#include "bailout.h"
#include "memory.h"
#include "log.h"
#include "http.h"
#include "server.h"
#include "params.h"

/*(*** server_stream_protocol_number */
int server_stream_protocol_number(void)
/* Do it by the book, compute "zero" */
{
  struct protoent *pe;
  
  pe = getprotobyname(PARAM_STREAM_PROTOCOL_NAME);
  if(!pe) bailout("Cannot get protocol entry for \"%s\"", PARAM_STREAM_PROTOCOL_NAME);
  return pe->p_proto;
}
/***)*/
/*(*** server_cxsk_reader */
void server_cxsk_reader(void *svd, connection *cn, short events)
{
  int fd;
  struct sockaddr_in sa;
  socklen_t sa_len;
  server *sv;
  http *ht;

  sv = svd;

  if(events & POLLIN) {
    sa_len = sizeof(sa);
    fd = accept(cn->cn_fd, (struct sockaddr *) &sa, &sa_len);
    if(fd < 0) {
      log_msg(sv->sv_log, LOG_WARN, "Accept returned an error (%s)", strerror(errno));
    } else {
      /* Get the numeric address of the internet connection */
      u_int32_t ip;
      int port;

      port = ntohs(sa.sin_port);
      ip = ntohl(sa.sin_addr.s_addr);
      log_msg(sv->sv_log, LOG_INFO, "Got connection into FD %d from %d.%d.%d.%d port %d",
              fd,
              (ip >> 24),
              (ip >> 16) & 255,
              (ip >> 8) & 255,
              ip & 255,
              port);

      if(fcntl(fd, F_SETFD, O_NONBLOCK) < 0)
        log_msg(sv->sv_log, LOG_ERROR, "Cannot make socket non-blocking (%s)", strerror(errno));

      /* Register connection with demultiplexer */
      cn = demux_add_connection(
             sv->sv_demux,
             fd,
             POLLIN,
             http_connection_handler,
             http_connection_delete,
             0);

      /* Create an HTTP handler */
      ht = http_create(sv, cn, &sa);
      demux_set_connection_data(cn, ht);

    }
  }
}
/***)*/
/*(*** server_start */
server *server_start(options *opt)
{
  server *sv;
  int protocol;
  struct sockaddr_in sa;

  /* Initialization and log file */
  sv = xmalloc(sizeof(server));
  sv->sv_log = log_init(opt);
  sv->sv_running = 1;
  sv->sv_interrupting_signal = -1;
  sv->sv_cycles = 0;

  /* Create a demuxer */
  sv->sv_demux = demux_create(sv);

  /*(*** Connection socket handling */
  /* Create and bind socket */
  protocol = server_stream_protocol_number();
  sv->sv_socket = socket(PF_INET, SOCK_STREAM, protocol);
  if(sv->sv_socket < 0) bailout_with_error(errno, "Cannot create socket");

  {
    int sopt;
    sopt = 1;
    if(setsockopt(sv->sv_socket, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt)) < 0)
      bailout_with_error(errno, "Cannot set SO_REUSEADDR");
  }
  if(fcntl(sv->sv_socket, F_SETFD, O_NONBLOCK) < 0)
    bailout_with_error(errno, "Cannot make socket non-blocking");

  /* Bind */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(opt->opt_port);
  sa.sin_addr.s_addr = INADDR_ANY;
  if(bind(sv->sv_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0)
    bailout_with_error(errno, "Cannot bind socket to port %d", opt->opt_port);

  /* Listen */
  if(listen(sv->sv_socket, opt->opt_backlog))
    bailout_with_error(errno, "Cannot listen with backlog of %d", opt->opt_backlog);
  /***)*/

  /* Register connection with demultiplexer */
  (void) demux_add_connection(sv->sv_demux, sv->sv_socket, POLLIN, server_cxsk_reader, 0, 0);

  /* Finish */
  log_msg(sv->sv_log, LOG_INFO, "Server started");
  return sv;
}
/***)*/
/*(*** server_shutdown */
void server_shutdown(server *sv)
{
  log_msg(sv->sv_log, LOG_INFO, "Server finishing");
  log_shutdown(sv->sv_log);
  xfree(sv);
}
/***)*/
/*(*** server_interrupt */
void server_interrupt(server *sv, int signal)
{
  sv->sv_running = 0;
  sv->sv_interrupting_signal = signal;
}
/***)*/
/*(*** server_run */
int server_run(server *sv)
{
  if(sv->sv_running) {
    /* Job */
    sv->sv_cycles ++;

    demux_poll_and_run(sv->sv_demux, sv->sv_log);

    return 1;
  } else {
    log_msg(sv->sv_log, LOG_INFO, "Server interrupted");
    return 0;
  }
}
/***)*/
