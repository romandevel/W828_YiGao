/** 
 * Function prototypes for handling the HTTP protocol.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _PROTOCOL_HTTP_H_
#define _PROTOCOL_HTTP_H_

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
#include "hyTypes.h"
#include "std.h"
#include "socket_api.h"
//#include "win.h"
//#include "Win_main.h"
//#include "parhelpers.h"
#include "mem_manage_api.h"
#include "lprotocol.h"
#include "IEdns.h"
#ifdef __cplusplus
extern "C" {
#endif


#define HYIE_MEM_IDX		0

#define HYWB_OK		0
#define HYWB_ERR  -1


enum http_request_method {
	HTTP_GET_METHOD=0,
	HTTP_POST_METHOD,
	HTTP_HEAD_METHOD,
	HTTP_OPTIONS_METHOD,
	HTTP_PUT_METHOD,
	HTTP_DELETE_METHOD,
	HTTP_TRACE_METHOD,
	HTTP_CONNECT_METHOD
};

typedef enum http_charset_type
{
	CHARSET_GBK=0,
	CHARSET_UTF8,
	//CHARSET_URL
	
}eHTTP_CHARSET_TYPE;

//发送http请求的头属性
struct protocol_http_request {
    enum http_request_method method;
    int  content_length;
    int  connection;
    int  gzip;
    char *user_agent;
    char *content_type;
    char *Content_Disposition;
    char *referer;
    char *content_data;
    char *range;
};

#define ISSPACE(ch)  ((unsigned int)(ch - 9) < 5u  ||  ch == ' ')
#define TOLOWER(ch)  (((unsigned int)((ch) - 'A') < 26u) ? (ch + 'a' - 'A') : (ch))
#define ISXDIGIT(ch) ((unsigned int)( ch - '0') < 10u || (unsigned int)((ch | 0x20) - 'a') < 6u)
#define STRLEN(str)  Strlen(str)
#define ATOI(str)    AtoI(str)



/* Function prototypes. */
extern int protocol_http_open(struct protocol_url *url, 
                       struct protocol_http_headers *headers,
                       struct protocol_http_request *request_headers);
extern int protocol_http_close(int fd);
extern int protocol_http_select(int sock, int sec, int usec, short type);


#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_HTTP_H_ */
