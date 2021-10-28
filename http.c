/* http.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

#include "params.h"
#include "scan.h"
#include "http.h"
#include "log.h"

int is_valid_digit(int c) { return isascii(c) && isdigit(c); }
int is_valid_alpha(int c) { return isascii(c) && isalpha(c); }
int is_valid_space(int c) { return c == ' ' || c == '\t'; }
int is_valid_uri_char(int c) { return isascii(c) && isprint(c) && !isspace(c); }
int is_valid_header_name_char(int c) { return isascii(c) && (isalnum(c) || c == '-' || c == '_'); }

/*(*** http_release_request_line */
void http_release_request_line(http_request_line *rl)
{
  if(rl->hrl_allocated) {
    xfree(rl->hrl_method);
    xfree(rl->hrl_uri);
    xfree(rl->hrl_major);
    xfree(rl->hrl_minor);
  }
}
/***)*/
/*(*** http_release_header_line */
void http_release_header_line(http_header_line *hl)
{
  if(hl->hhl_allocated) {
    hl->hhl_allocated = 0;
    xfree(hl->hhl_name);
    lines_delete(hl->hhl_value);
  }
}
/***)*/
/*(*** http_parse_header_line */
int http_parse_header_line(http_header_line *hl, char *p, int m)
{
  scanbuf sb;
  int i;
  int name_i, name_j;
  int value_i, value_j;
  char *name;
  lines *value;

  scan_init(&sb, p, m);

#define check(x) if((x) < 0) return 0;

  name_i   = 0;
  name_j   = scan_alphabet_plus  (&sb, name_i, is_valid_header_name_char);   check(name_j);
  i        = scan_char           (&sb, name_j, ':');                         check(i);
  value_i  = scan_alphabet_plus  (&sb, i, is_valid_space);                   check(value_i);
  value_j  = sb.sb_length;

#undef check

  name     = scan_extract(&sb, name_i, name_j - 1);
  value    = lines_create(0, 0);
  lines_add_bytes(value, sb.sb_data + value_i, value_j - value_i);

  hl->hhl_allocated = 1;
  hl->hhl_name = name;
  hl->hhl_value = value;

  return 1;
}
/***)*/
/*(*** http_parse_header_continuation_line */
int http_parse_header_continuation_line(http_header_line *hl, char *p, int m)
{
  scanbuf sb;
  int value_i, value_j;

  scan_init(&sb, p, m);

#define check(x) if((x) < 0) return 0;

  value_i  = scan_alphabet_plus  (&sb, 0, is_valid_space);                   check(value_i);
  value_j  = sb.sb_length;

#undef check

  lines_add_bytes(hl->hhl_value, sb.sb_data + value_i, value_j - value_i);

  return 1;
}
/***)*/
/*(*** http_parse_request_line */
int http_parse_request_line(http_request_line *rl, char *p, int m)
{
  scanbuf sb;
  int i;
  int method_i, method_j;
  int uri_i, uri_j;
  int major_i, major_j;
  int minor_i, minor_j;

  char *method;
  char *uri;
  char *major, *minor;

  scan_init(&sb, p, m);

#define check(x) if((x) < 0) return 0;

  method_i = 0;
  method_j = scan_alphabet_plus  (&sb, method_i, is_valid_alpha);   check(method_j);
  uri_i    = scan_alphabet_plus  (&sb, method_j, is_valid_space);   check(uri_i);
  uri_j    = scan_alphabet_plus  (&sb, uri_i, is_valid_uri_char);   check(uri_j);
  i        = scan_alphabet_plus  (&sb, uri_j, is_valid_space);      check(i);
  major_i  = scan_convert_nulstr (&sb, i, toupper, "HTTP/");        check(i);
  major_j  = scan_alphabet_plus  (&sb, major_i, is_valid_digit);    check(major_j);
  minor_i  = scan_char           (&sb, major_j, '.');               check(minor_i);
  minor_j  = scan_alphabet_plus  (&sb, minor_i, is_valid_digit);    check(minor_j);
  i        = scan_end            (&sb, minor_j);                    check(i);

  method   = scan_extract(&sb, method_i, method_j - 1);
  uri      = scan_extract(&sb, uri_i,    uri_j    - 1);
  major    = scan_extract(&sb, major_i,  major_j  - 1);
  minor    = scan_extract(&sb, minor_i,  minor_j  - 1);

#undef check

  scan_upperify(method);

  rl->hrl_allocated = 1;
  rl->hrl_method    = method;
  rl->hrl_uri       = uri;
  rl->hrl_major     = major;
  rl->hrl_minor     = minor;

  return 1;
}
/***)*/
/*(*** http_uri_to_filename */
char *http_uri_to_filename(http *ht, char *uri)
{
  /* Unimplemented and dirty. XXX */
  return strdup(uri);
}
/***)*/
/*(*** http_process_header */
static int http_process_header(http *ht)
{
  /* Process the header contained in ht_hhl.
   * Currently does nothing.
   */
  if(ht->ht_hhl.hhl_allocated) {
    char *value;
    value = lines_contents(ht->ht_hhl.hhl_value);
    log_msg(ht->ht_server->sv_log,
            LOG_DEBUG,
            "Header name is [%s] value is [%s]",
            ht->ht_hhl.hhl_name,
            value);
    xfree(value);
  }
  return 1;
}
/***)*/
/*(*** http_eater */
void http_eater(void *htd, char *p, int m)
{
  http *ht;
  server *sv;

  ht = htd;
  sv = ht->ht_server;

#if DEBUG
  {
    char buf[75];
    int i;
    for(i = 0; i < m && i < sizeof(buf) - 1; i ++) {
      buf[i] = isprint(p[i]) ? p[i] : '?';
    }
    buf[i] = 0;
    log_msg(sv->sv_log, LOG_DEBUG, "Got line [%s]", buf);
  }
#endif

reswitch:
  switch(ht->ht_state) {
    case HTTP_REQUEST:
      /* XXX: parse request */
      {
        if(!http_parse_request_line(&ht->ht_hrl, p, m)) goto violation; /* MEMORY LEAK */

        log_msg(sv->sv_log,
                LOG_DEBUG,
                "Method is [%s] URI is [%s] major is [%s] minor is [%s]",
                ht->ht_hrl.hrl_method,
                ht->ht_hrl.hrl_uri,
                ht->ht_hrl.hrl_major,
                ht->ht_hrl.hrl_minor);

        ht->ht_state = HTTP_HEADER;
      }
      break;
    case HTTP_HEADER:
      if(m) {
        /* Non-empty header lines */
        http_release_header_line(&ht->ht_hhl);
        if(http_parse_header_line(&ht->ht_hhl, p, m)) {
          /* This is a nice header */
          ht->ht_state = HTTP_MORE_HEADERS;
        } else goto violation;
      } else {
        /* Empty line : headers finished */
        if(!http_process_header(ht)) goto violation;
        ht->ht_state = HTTP_BUILD_RESPONSE;
        goto reswitch;
      }
      break;
    case HTTP_MORE_HEADERS:
      if(m) {
        if(http_parse_header_continuation_line(&ht->ht_hhl, p, m))
          break; /* Stay in this state. */
        else {
          /* Not a continuation header. */
          /* Post current header. */
          if(!http_process_header(ht)) goto violation;
          ht->ht_state = HTTP_HEADER;
          goto reswitch;
        }
      } else {
        /* Empty line : headers finished */
        if(!http_process_header(ht)) goto violation;
        ht->ht_state = HTTP_BUILD_RESPONSE;
        goto reswitch;
      }
      break;
    case HTTP_BUILD_RESPONSE:
      /* Build response */
      {
        char *response;
        int response_length;
        char *content_type;
        size_t content_length;
        resource *rs;
        char *fn;

        if(!strcmp(ht->ht_hrl.hrl_method, "GET")) {
          fn = http_uri_to_filename(ht, ht->ht_hrl.hrl_uri);

          rs = resource_obtain(fn);
          if(!rs) {
            log_msg(sv->sv_log, LOG_ERROR, "Can't obtain %s (%s)", fn, strerror(errno));

            response_length =
              xasprintf(
                &response,
                "HTTP/1.1 404 Not Found\r\n"
                  "Content-Type: text/html\r\n"
                  "Connection: close\r\n"
                  "\r\n"
                  "<html>"
                  "<head><title>404 Not Found</title></head><body>"
                  "Unable to find resource %s (%s)."
                  "</body></html>", 
                fn,
                strerror(errno));
            ht->ht_state = HTTP_RESPONSE_ONLY;
          } else {
            content_type = rs->rs_mime_type;
            content_length = rs->rs_length;

            if(ht->ht_writer) { 
              writer_delete(ht->ht_writer);
              ht->ht_writer = 0;
            }
            ht->ht_resource = rs;

            response_length =
              xasprintf(
                &response,
                "HTTP/1.1 200 OK\r\n"
                  "Content-Type: %s\r\n"
                  "Connection: close\r\n"
                  "Content-Length: %d\r\n"
                  "\r\n",
                content_type,
                content_length);
            ht->ht_state = HTTP_RESPONDING;
          }
          xfree(fn);
        } else {
          response_length =
            xasprintf(
              &response,
              "HTTP/1.1 501 Not implemented\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html>"
                "<head><title>501 Not implemented</title></head><body>"
                "Method %s is not supported by this server."
                "</body></html>", 
              ht->ht_hrl.hrl_method);
          ht->ht_state = HTTP_RESPONSE_ONLY;
        }
        ht->ht_response = response;
        ht->ht_writer = writer_create(response, response_length, response);
        demux_set_event_mask(sv->sv_demux, ht->ht_connection, POLLOUT); /* Stop reading */
      }
      break;
    case HTTP_RESPONSE_ONLY:
    case HTTP_RESPONDING:
      break;
    case HTTP_SERVING:
      break;
  }
  return;

violation:
  log_msg(sv->sv_log, LOG_ERROR, "HTTP protocol violation");
  demux_delete_connection(sv->sv_demux, ht->ht_connection);
  return;
}
/***)*/
/*(*** http_create */
http *http_create(server *sv, connection *cn, struct sockaddr_in *sa)
{
  http *ht;

  ht = xmalloc(sizeof(http));
  ht->ht_sockaddr = *sa;
  ht->ht_server = sv;
  ht->ht_connection = cn;
  ht->ht_lines = lines_create(http_eater, ht);
  ht->ht_state = HTTP_REQUEST;
  ht->ht_response = 0;
  ht->ht_hhl.hhl_allocated = 0;
  ht->ht_hrl.hrl_allocated = 0;
  ht->ht_writer = 0;
  ht->ht_resource = 0;
  return ht;
}
/***)*/
/*(*** http_delete */
void http_delete(http *ht)
{
  lines_delete(ht->ht_lines);
  if(ht->ht_response) xfree(ht->ht_response);
  if(ht->ht_hrl.hrl_allocated) http_release_request_line(&ht->ht_hrl);
  if(ht->ht_writer) writer_delete(ht->ht_writer);
  if(ht->ht_resource) resource_release(ht->ht_resource);
  xfree(ht);
}
/***)*/
/*(*** http_add_bytes */
void http_add_bytes(http *ht, char *p, int m)
{
  lines_add_bytes(ht->ht_lines, p, m);
}
/***)*/
/*(*** http_connection_handler */
void http_connection_handler(void *svd, connection *cn, short events)
{
  server *sv;
  http *ht;

  sv = svd;
  ht = cn->cn_data;

  log_msg(sv->sv_log, LOG_DEBUG, "Event mask on FD %d is 0x%04x", cn->cn_fd, events);
  if(events & POLLERR) {
    demux_delete_connection(sv->sv_demux, cn);
    log_msg(sv->sv_log, LOG_WARN, "Deleting connection on FD %d", cn->cn_fd);
    return;
  }
  if(events & POLLIN) {
    int fd;
    int r;
    char buf[PARAM_READ_BUFFER_SIZE];

    fd = cn->cn_fd;
    r = read(fd, buf, sizeof(buf));
    if(r < 0) {
     log_msg(sv->sv_log, LOG_DEBUG, "Read error on FD %d (%s)", cn->cn_fd, strerror(errno));
    } else {
      if(r > 0) {
        log_msg(sv->sv_log, LOG_DEBUG, "Read %d bytes on FD %d", r, cn->cn_fd);
        http_add_bytes(ht, buf, r);
      } else {
       log_msg(sv->sv_log, LOG_INFO, "Connection closed on FD %d", cn->cn_fd);
       if(close(fd) < 0) {
         log_msg(sv->sv_log, LOG_ERROR, "Error closing FD %d (%s)", cn->cn_fd, strerror(errno));
       }
       demux_delete_connection(sv->sv_demux, cn);
      }
    }
  }
  if(events & POLLOUT) {
    switch(ht->ht_state) {
      case HTTP_SERVING:

      case HTTP_RESPONDING:
      case HTTP_RESPONSE_ONLY:
        switch(writer_work(ht->ht_writer, cn->cn_fd)) {
          case WRITER_WORKING:
            break;
          case WRITER_DONE:
            switch(ht->ht_state) {
              case HTTP_RESPONDING:
                ht->ht_state = HTTP_SERVING;
                writer_delete(ht->ht_writer);
                ht->ht_writer = writer_create(ht->ht_resource->rs_start, ht->ht_resource->rs_length, 0);
                break;
              case HTTP_RESPONSE_ONLY:
              default:
                demux_delete_connection(sv->sv_demux, cn);
                break;
            }
            break;
          case WRITER_ERROR:
            log_msg(sv->sv_log, LOG_ERROR, "Error writing to FD %d (%s)", cn->cn_fd, strerror(errno));
            demux_delete_connection(sv->sv_demux, cn);
            break;
        }
        break;
      default:
        break;
    }
  }
}
/***)*/
/*(*** http_connection_delete */
void http_connection_delete(void *svd, connection *cn)
{
  http *ht;

  ht = cn->cn_data;

  http_delete(ht);
}
/***)*/
