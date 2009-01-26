/* http.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef HTTP_H
#define HTTP_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "demux.h"
#include "writer.h"
#include "lines.h"
#include "resource.h"

typedef enum {
  HTTP_REQUEST,
  HTTP_HEADER,
  HTTP_MORE_HEADERS,
  HTTP_BUILD_RESPONSE,
  HTTP_RESPONDING,
  HTTP_RESPONSE_ONLY,
  HTTP_SERVING
} http_state;

typedef struct {
  int hrl_allocated;
  char *hrl_method;
  char *hrl_uri;
  char *hrl_major;
  char *hrl_minor;
} http_request_line;

typedef struct {
  int hhl_allocated;
  char *hhl_name;
  lines *hhl_value;
} http_header_line;

typedef struct {
  server *ht_server;
  connection *ht_connection;
  lines *ht_lines;
  struct sockaddr_in ht_sockaddr;
  http_state ht_state;
  char *ht_response; /* Allocated HTTP response. */
  http_request_line ht_hrl;
  http_header_line ht_hhl;
  writer *ht_writer;
  resource *ht_resource;
} http;

http *http_create(server *sv, connection *cn, struct sockaddr_in *sa);
void http_delete(http *ht);
void http_add_bytes(http *ht, char *p, int m);
void http_connection_handler(void *svd, connection *cn, short events);
void http_connection_delete(void *svd, connection *cn);
#endif
