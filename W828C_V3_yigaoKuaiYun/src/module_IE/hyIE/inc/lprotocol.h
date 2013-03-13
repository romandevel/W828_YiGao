/** 
 * Function prototypes and structures for handling the streams used by the 
 * program to read data from.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _PROTOCOL_PROTOCOL_H_
#define _PROTOCOL_PROTOCOL_H_

/*
 * Copyright (C) 1999, Tomas Berndtsson <tomas@nocrew.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//#include "win.h"
#include "streams.h"
#include "file.h"
#include "http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Contains the different parts of a URL, for easier handling
 * and storing. These things are delicate creatures, you know.
 *
 * @member type The protocol type, without the tailing colon and
 * @member type double slashes, or NULL if not given.
 * @member user The authorization username, or NULL if not given.
 * @member pass The authorization password, or NULL if not given.
 * @member host The full hostname or IP number, or NULL if not given.
 * @member port The port number to connect to. This is guessed, depending
 * @member port on the protocol type, if not given in the URL.
 * @member file The directory and filename, without the leading slash,
 * @member file or NULL if not given.
 */
struct protocol_url {
  char *type;
  char *user;
  char *pass;
  char *host;
  unsigned int port;
  char *file;
};

/**
 * Contains information taken from the HTTP response headers.
 *
 * @member return_code The return code for the request.
 * @member return_message The return message, if there was one together
 * @member return_message with the return code.
 * @member content_type_major The major content type string, if one was 
 * @member content_type_major provided for this stream. NULL otherwise.
 * @member content_type_minor The minor content type string, if one was 
 * @member content_type_minor provided for this stream. NULL otherwise.
 * @member charset The charset to be used for this page. It is currently
 * @member charset not used, but ISO-8859-1 is always assumed. 
 * @member content_length The length of the data in the stream, or a negative
 * @member content_length if it was not given in the headers.
 * @member server A string identifying the server we are currently talking to.
 * @member server NULL, if the server wished to remain anonymous.
 * @member location The new location of a temporarily moved page, or NULL if
 * @member location we found the real page.
 * @member real_url Not really a response header, but it seems approrpiate to 
 * @member real_url store the real URL which was used for reading here.
 */
struct protocol_http_headers {
  int return_code;
  char *return_message;
  char *content_type_major;
  char *content_type_minor;
  char *charset;
  char *content_encoding;
  char *connection;
  unsigned int content_length;
  char *server;
  char *location;
  char *real_url;
  char *cookie;
  char *TransferEncoding;
};

//session id
struct session_id{
    char *host;               //Ö÷»úµØÖ·
    char *id;                 //sessionid×Ö·û´®
};

/* Function prototypes. */
//extern int protocol_open(char *url, char *base_url, struct protocol_http_request *request_headers);
extern int  protocol_store_SID(char *host, char *id);
extern int protocol_search_SID(char *host);
extern void protocol_init(void);
extern int  free_httpHeader(void);
extern int  protocol_close(int fd);
extern int  protocol_get_type(int fd);
extern int  protocol_store_base(char *url);
extern void protocol_free_url(struct protocol_url *url);
extern unsigned int protocol_default_port(char *protocol);
extern struct protocol_url *protocol_split_url(char *url);
extern struct protocol_stream *protocol_get_stream(int fd);
extern char *protocol_unsplit_url(struct protocol_url *url);
extern char *protocol_make_absolute(char *url, char *base_url);
extern int  protocol_recv(int type, void *handle, char *buf, int len);
extern void protocol_free_headers(struct protocol_http_headers *headers);
extern struct protocol_http_headers *protocol_get_headers(int fd,char *url);
extern int  protocol_send(int type, void *handle, char *buf, int len);
#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_PROTOCOL_H_ */
