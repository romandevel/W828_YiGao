/**
 * Functions to provide a generic method of getting a stream from 
 * any supported protocol, according to the URL.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

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

#include "lprotocol.h"
#include "https.h"

#include "socket_api.h"

//#ifndef NO_WEB_DEBUG
/**
 * A pointer to the first element in the linked list containing the 
 * information about the streams. 
 */
static struct protocol_stream *first_stream = NULL;

/* Used to store the current base URL. */
static char *base_host = NULL;
static char *base_path = NULL;

/* JSESSIONID */
#define MAX_SESSIONID_SIZE    10         //暂支持最多10组sessionid
static int session_cnt;
struct session_id session[MAX_SESSIONID_SIZE];
void protocol_destroy_SID(void);

void protocol_init(void)
{
	first_stream = NULL;
	base_host = NULL;
	base_path = NULL;
	session_cnt = 0;
	memset(session, '\0', MAX_SESSIONID_SIZE*sizeof(struct session_id));
}

void protocol_destroy(void)
{
    FREEBUFFER(base_host,HYIE_MEM_IDX);
    FREEBUFFER(base_path,HYIE_MEM_IDX);
    protocol_destroy_SID();
}

/**
 * Store a URL as the base for following relative references. This will
 * store the URL in two parts, the host, including protocol type, and
 * the path, not including the initial slash.
 *
 * @param url The full URL to make the new base URL.
 *
 * @return non-zero value if an error occurred.
 */
 int protocol_store_base(char *url)
{
  char *host_text, *path_text, *tmp;

  host_text = (char *)GETBUFFER(STRLEN(url) + 16,HYIE_MEM_IDX);
  if(host_text == NULL)
    return HYWB_ERR;
  strcpy(host_text, url);

  if(!hy_memicmp(host_text, "http://", 7) || !hy_memicmp(host_text, "https://", 8)) {
    /* If the URL contains GET method information after a question mark, 
     * we will need to remove that first. We search for the first occurrance
     * of a question mark and remove everything efter that. 
     */
    tmp = strchr(host_text, '?');
    if(tmp != NULL)
      tmp[0] = '\0';

    path_text = strchr(&host_text[7], '/');
    if(path_text != NULL) {
      path_text[0] = '\0';
      path_text++;
    }
	
	base_host = REGETBUFFER( base_host, STRLEN(host_text) + 1 ,HYIE_MEM_IDX);
    strcpy(base_host, host_text);
  } else if(!hy_memicmp(host_text, "file://", 7)) {
    path_text = host_text + 7;

	//	ATTENTION! IF USE FILE PROTOCOL ,WE ONLY GET FILE FROM LOCAL DISK, 
	//	SO I JUST SETTEH " base_host = NULL",THIS IS NOT A STRICK CODE.
	//	IF THE ENVIROMENT CHANGES ,THIS MAYBE UPDATE! 

   /* 
	base_host = REGETBUFFER(base_host, 16);
    strcpy(base_host, "file://");
   */

	 base_host = NULL;

  } else {
    base_host = REGETBUFFER(base_host, 16,HYIE_MEM_IDX);
    //strcpy(base_host, "file://");

    base_host = NULL;
	path_text = host_text;
  }

  /* If we have a path, copy it to base_path, otherwise set the same
   * to an empty string.
   */
  if(path_text != NULL) {

    /* If the last element in the URL is a file, we want to remove that. 
     * At this point, we know that the URL will always end with a slash, 
     * if the last element is a directory. We have ensured that when
     * trying to connect to the URL.
     *
     * Hmm. This might not be true for other protocols than HTTP.
     */

    if(path_text[STRLEN(path_text) - 1] != '/') {
 
		tmp = strrchr(path_text, '/');

		if(tmp == NULL)
	tmp = path_text;
      tmp[0] = '\0';
    }

    base_path = REGETBUFFER(base_path, STRLEN(path_text) + 1,HYIE_MEM_IDX);
    if(base_path == NULL)
      return 1;
    strcpy(base_path, path_text);    
  } else {
    base_path = REGETBUFFER(base_path, 1,HYIE_MEM_IDX);
    if(base_path == NULL)
      return 1;
    base_path[0] = '\0';
  }
  
  FREEBUFFER(host_text,HYIE_MEM_IDX);

  return HYWB_OK;
}

/**
 * Take a URL and give it an awful treatment, to squeeze the size to its
 * very limits. The lower limits. More detailed, this will remove double,
 * or more, slashes, slash dot slash and slash dot dot together with the 
 * previous directory. We do not remove slash dot at the end of a URL,
 * because Roxen web server uses this in a feature to list a directory
 * where index.html, or similar, normally would have been loaded.
 * This function expects an absolute URL.
 *
 * @param url The uncompressed URL.
 *
 * @return non-zero value if an error occurred.
 */
static int protocol_compress_url(char *url)
{
  char *dir_start, *tmp, *tmp1, *tmp2;
  
  if(!hy_memicmp(url, "http://", 7)) {
    dir_start = strchr(&url[7], '/');
    if(dir_start == NULL)
      return HYWB_OK;
  } else if(!hy_memicmp(url, "file://", 7)) {
    dir_start = &url[7];
  } else {
    return 1;
  }

  /* Start with removing all multiple slashes. j.w.li_080109服务器端程序转接url时,不能压缩.如asp?http://情况*/
  tmp1 = (char *)strstr(dir_start, "?");
  while((tmp = (char *)strstr(dir_start, "//")) != NULL) {
     if (tmp1!=NULL && tmp1<tmp)
         break;
     memmove(tmp, &tmp[1], STRLEN(tmp));
  }
  
  /* Then remove slash dot slash. */
  while((tmp = (char *)strstr(dir_start, "/./")) != NULL) {
    memmove(tmp, &tmp[2], STRLEN(tmp) - 1);
  }

  /* And almost last, but almost not least, remove slash dot dot slash
   * and previous directory.  
   */
  while((tmp = (char *)strstr(dir_start, "/../")) != NULL) {
    tmp2 = tmp;
    tmp2--;
    while(tmp2 >= dir_start && *tmp2 != '/') {
      tmp2--;
    }
    if(tmp2 < dir_start)
      memmove(tmp, &tmp[3], STRLEN(tmp) - 2);
    else
      memmove(tmp2, &tmp[3], STRLEN(tmp) - 2);
  }  

  /* Last case, which is a special treatment of slash dot dot, at the
   * end of a URL.
   */
  tmp = (char *)strstr(dir_start, "/..");
  if(tmp != NULL && tmp[3] == '\0') {
    tmp2 = tmp;
    tmp2--;
    while(tmp2 >= dir_start && *tmp2 != '/') {
      tmp2--;
    }
    if(tmp2 < dir_start)
      memmove(tmp, &tmp[3], STRLEN(tmp) - 2);
    else
      memmove(tmp2, &tmp[3], STRLEN(tmp) - 2);    
  }

  return HYWB_OK;
}

/**
 * This takes a URL, absolute or relative, and tries to create an 
 * absolute URL out of it, and the stored base URL. 
 *
 * @param url The absolute or relative URL to absolutify.
 * @param base_url If not NULL, this is used as the new base URL.
 *
 * @return the new, absolutely absolute URL.
 */
char *protocol_make_absolute(char *url, char *base_url)
{
  char *new_url, *tmp;
  int alloc_size;//, cwd_size;
  char *cwd;

  /* If we are given a completely non-existing URL (It is possible
   * for example if we get an empty src="" option in an img tag.) 
   * we just exit. 
   */
  if(url == NULL)
    return NULL;

  /* First of all, we check if an unsupported protocol is being used.
   * If it is, we do not proceed. 
   * This is a bad method, since we cannot possible list all protocols
   * we will not support. The problem is that if a known protocol is
   * not specified, we want to try to access it as a file.
   */
  if(!hy_memicmp(url, "ftp://", 6) ||
     !hy_memicmp(url, "mailto:", 7) ||
     !hy_memicmp(url, "telnet://", 9)) {
#ifdef WB_DEBUG
    fprintf(stderr, "Unsupported protocol used for '%s'.\n", url);
#endif
    return NULL;
  }

  /* If a new base URL was provided, we set this as the current base URL. */
  if(base_url)
    protocol_store_base(base_url);

  cwd = NULL;

  if(!hy_memicmp(url, "http:/", 6) ||//j.w.li_1224 return_code=302时,location=http:/xxxx样式
     !hy_memicmp(url, "file:/", 6)) {
    /* New absolute URL. */
    if (url[6] != '/')
    {
        new_url = (char *)GETBUFFER(STRLEN(url) + 2,HYIE_MEM_IDX);
        if(new_url == NULL)
            return NULL;
        strncpy(new_url, url, 6);
        new_url[6] = '/';
        strcpy(new_url+7, url+6);
    }
    else
    {
        new_url = (char *)GETBUFFER(STRLEN(url) + 1,HYIE_MEM_IDX);
        if(new_url == NULL)
          return NULL;
        strcpy(new_url, url);
    }
  }else if (!hy_memicmp(url, "https:/", 7))
  {
      new_url = (char *)GETBUFFER(STRLEN(url) + 1,HYIE_MEM_IDX);
      if(new_url == NULL)
          return NULL;
      strcpy(new_url, url);
  }else if(url[0] == '/') {
    /* New path only. */
    if(base_host)
      alloc_size = STRLEN(base_host) + STRLEN(url) + 1;
    else
      alloc_size = 7 + STRLEN(url) + 1;

    new_url = (char *)GETBUFFER(alloc_size,HYIE_MEM_IDX);
    if(new_url == NULL)
      return NULL;

    if(base_host)
      strcpy(new_url, base_host);
    else
      strcpy(new_url, "file://");
    strcat(new_url, url);

  } else if (url[0] == '#'){ //'#'为wml中的程序段锚点,在同一卡片组中定位不同的卡片
      /*if ( a_Nav_stack_ptr(gns) >= 0 )
      {   
          tmp = a_History_get_url(NAV_IDX(gns,gns->nav_stack_ptr));
          alloc_size = STRLEN(tmp) + 1 + STRLEN(url);
          new_url = (char *)GETBUFFER(alloc_size);
          if (new_url == NULL)
              return NULL;
          strcpy(new_url, tmp);
          strcat(new_url, url);
      }
      else       
      {
          if (base_host)
            alloc_size = STRLEN(base_host) + 1;
          else
            alloc_size = STRLEN(url) + 1;
          new_url = (char *)GETBUFFER(alloc_size);
          if (new_url == NULL)
              return NULL;
          if (base_host)
            strcpy(new_url, base_host);
          else
            strcpy(new_url, url);
      }*/
  }
  else {
    /* Relative path. */
	int cwd_size = 0;
    alloc_size = STRLEN(url) + 4;
    if(base_host)
      alloc_size += STRLEN(base_host);
    else
      alloc_size += 7;
    if(base_path) {
      alloc_size += STRLEN(base_path);
    }
  
    new_url = (char *)GETBUFFER(alloc_size,HYIE_MEM_IDX);
    if(new_url == NULL)
      return NULL;

    if(base_host) 
    {
      strcpy(new_url, base_host);
      strcat(new_url, "/");
    } 
    else if (!hy_memicmp(base_path, "C:/", 3))   //判断是否为本地文件
    {
      strcpy(new_url, "file://");
    }
    else
    {
      strcpy(new_url, "http:/");
    }  

    if(base_path) {
      strcat(new_url, base_path);
    }
    strcat(new_url, "/");
    strcat(new_url, url);
  }

  protocol_compress_url(new_url);

  if(cwd != NULL)
    FREEBUFFER(cwd,HYIE_MEM_IDX);

  return new_url;
}

/**
 * Frees everything that has been allocated for the HTTP headers.
 *
 * @param headers A pointer to the headers to free.
 */
void protocol_free_headers(struct protocol_http_headers *headers)
{
	if(headers == NULL)
		return;

	if(headers->return_message)
		FREEBUFFER(headers->return_message,HYIE_MEM_IDX);
	if(headers->content_type_major)
		FREEBUFFER(headers->content_type_major,HYIE_MEM_IDX);
	if(headers->content_type_minor)
		FREEBUFFER(headers->content_type_minor,HYIE_MEM_IDX);
	if(headers->server)
		FREEBUFFER(headers->server,HYIE_MEM_IDX);
	if(headers->location)
		FREEBUFFER(headers->location,HYIE_MEM_IDX);
	if(headers->real_url)
		FREEBUFFER(headers->real_url,HYIE_MEM_IDX);
	if(headers->charset)
		FREEBUFFER(headers->charset,HYIE_MEM_IDX);
	if(headers->content_encoding)
		FREEBUFFER(headers->content_encoding,HYIE_MEM_IDX);
	if(headers->connection)
		FREEBUFFER(headers->connection,HYIE_MEM_IDX);
	if(headers->cookie)
		FREEBUFFER(headers->cookie,HYIE_MEM_IDX);
	if(headers->TransferEncoding)
		FREEBUFFER(headers->TransferEncoding,HYIE_MEM_IDX);

	FREEBUFFER(headers,HYIE_MEM_IDX);
  
}

/**
 * Gracefully closes a stream using the specified protocol scheme. 
 * This is used when an error has occurred, and the file descriptor
 * is not yet on the linked list of streams.
 *
 * @param fd The file descriptor of the stream to close.
 * @param protocol The protocol to use to close this stream.
 *
 * @return non-zero value if an error occurred.
 */
static int protocol_close_specific(int fd, enum protocol_type protocol)
{
  switch(protocol) {
  case PROTOCOL_HTTP:
	protocol_http_close(fd);
    break;

  case PROTOCOL_FILE:
 //   protocol_file_close(fd);
    break;

  default:
    return 1;
  }

  return HYWB_OK;
}

/**
 * After achieve a stream from the URL, add it to "first_stream" 
 *
 * @param fd The file descriptor of the stream to close.
 * @param protocol The protocol to use to close this stream.
 *
 * @return non-zero value if an error occurred.
 */
int protocol_add_stream(int fd, enum protocol_type protocol, struct protocol_http_headers *headers)
{
    struct protocol_stream *new_stream;
#ifdef HAVE_SSL
    extern SSL *pStream_ssl;
#endif

    new_stream = (struct protocol_stream *)
        GETBUFFER(sizeof(struct protocol_stream),HYIE_MEM_IDX);
    if (new_stream == NULL)
        return HYWB_ERR;

#ifdef HAVE_SSL
    new_stream->ssl = pStream_ssl;
#endif
    new_stream->fd = fd;
    new_stream->protocol = protocol;
    new_stream->headers = headers;
    new_stream->next = first_stream;
    first_stream = new_stream;
    return HYWB_OK;
}


int protocol_create(char *url)
{
	int fd;
	char *new_url;
	enum protocol_type protocol;
	struct protocol_url *url_parts=NULL;
	
	/* Get the absolute equivalence to the specified URL. */
	new_url = protocol_make_absolute(url, NULL);
	
	protocol = PROTOCOL_UNKNOWN;
	if(new_url)
	{
		url_parts = protocol_split_url(new_url);//分析url地址的组成
		if(url_parts) 
		{
			/* Find the correct protocol to use. */
			if(!_stricmp(url_parts->type, "http") || !_stricmp(url_parts->type, "https")) 
			{
				fd =(int)protocol_http_open_connection(url_parts);
				if(fd >= 0)
				{
					if (!_stricmp(url_parts->type, "http"))
						protocol = PROTOCOL_HTTP;
					else
						protocol = PROTOCOL_HTTPS;
				}
			}
			else if(!_stricmp(url_parts->type, "file")) 
			{
				protocol = PROTOCOL_FILE;
			}
			else
			{
			#ifdef WB_DEBUG
				fprintf(stderr, "Unsupported protocol used for '%s'\n", new_url);
			#endif
				fd = HYWB_ERR;
			}
		}
		else 
		{
			fd = HYWB_ERR;
		}
	}
	else
	{
		fd = HYWB_ERR;
	}
	
	FREEBUFFER(new_url,HYIE_MEM_IDX);
	protocol_free_url(url_parts);
	
	return fd;
}


/**
 * Takes a good look at the specified URL, looks deeply into its darkest
 * attributes and corners of significancy. After that, it tries its best
 * to decide on which protocol to use to achieve a stream from the URL.
 *
 * @param url The URL to open a stream from.
 * @param referer The URL we got from to get here, or NULL if jumping here.
 * @param base_url The base URL to be used to create an absolute URL from 
 * @param base_url the given URL, or NULL to use the locally stored base.
 * @param method   http 的get或post方式选择
 * @param content_type  http header 的 content-type 属性，在form进行发送数据时比较重要
 *
 * @return a file descriptor for the stream, or a negative value if an
 * @return error occurred.
 */
int protocol_open(char *url, char *base_url, struct protocol_http_request *request)
{
  int fd;
  char *new_url;
  enum protocol_type protocol;
  struct protocol_http_headers *headers;
  struct protocol_url *url_parts=NULL;

  headers = (struct protocol_http_headers *)
    GETBUFFER(sizeof(struct protocol_http_headers),HYIE_MEM_IDX);
  if(headers == NULL)
  {
  	//hyUsbPrintf("headers malloc fail!\r\n");
    return HYWB_ERR;
  }

  /* Fill in the default values for the HTTP headers struct. */
  headers->return_code = 200;
  headers->return_message = NULL;
  headers->content_type_major = NULL;
  headers->content_type_minor = NULL;
  headers->content_length = -1;
  headers->server = NULL;
  headers->location = NULL;
  headers->real_url = NULL;
  headers->charset = NULL;
  headers->content_encoding = NULL;
  headers->connection = NULL;
  headers->cookie = NULL;
	headers->TransferEncoding   = NULL;

  if(request->referer)
    protocol_store_base(request->referer);

  /* Get the absolute equivalence to the specified URL. */
  new_url = protocol_make_absolute(url, base_url);
  
  hyUsbPrintf("Absolute URL=%s!\r\n", new_url);

  protocol = PROTOCOL_UNKNOWN;
  if(new_url)
  {
		url_parts = protocol_split_url(new_url);//分析url地址的组成
		if(url_parts) 
		{
			/* Find the correct protocol to use. */
			if(!_stricmp(url_parts->type, "http") || !_stricmp(url_parts->type, "https")) 
			{
				fd =(int)protocol_http_open(url_parts, headers, request);//发送http请求,回复ok
				if(fd >= 0)
				{
					hyUsbPrintf("http open ok!\r\n");
					if (!_stricmp(url_parts->type, "http"))
						protocol = PROTOCOL_HTTP;
					else
						protocol = PROTOCOL_HTTPS;
				}
				else
				{
					//hyUsbPrintf("http open fail!\r\n");
				}
			}
			else if(!_stricmp(url_parts->type, "file")) 
			{
				//fd = protocol_file_open(url_parts->file, headers);
				protocol = PROTOCOL_FILE;
			}
			else
			{
			#ifdef WB_DEBUG
				fprintf(stderr, "Unsupported protocol used for '%s'\n", new_url);
			#endif
				fd = HYWB_ERR;
			}
		}
		else 
		{
			fd = HYWB_ERR;
		}
  }
  else
  {
    fd = HYWB_ERR;
  }

  /* Unless there was an unexpected error, we put the new file descriptor
   * and information about it onto the linked list. Its much easier to put
   * it at the top of the list, so we do that.
   */
  if(fd < 0) {
    if(new_url)
      FREEBUFFER(new_url,HYIE_MEM_IDX);
    protocol_free_url(url_parts);
    return fd;
  }

  /* This is not really right. */
  if(headers->return_code != 200 && headers->return_code != 206) 
  {
	if(new_url)
	  FREEBUFFER(new_url,HYIE_MEM_IDX);
	
	if (headers->return_code == 100)//释放资源，处理上传(如此处理，权益之计，对于网络方面需要调整的地方较多)
	{	
		protocol_free_url(url_parts);
    	return fd;
	}

	protocol_close_specific(fd, protocol);
	protocol_free_url(url_parts);
    return HYWB_ERR;
  }
hyUsbPrintf("protocol_add_stream---- \r\n");
  if (protocol_add_stream(fd, protocol, headers) == HYWB_ERR)
  {
      FREEBUFFER(new_url,HYIE_MEM_IDX);
      protocol_close_specific(fd, protocol);
      protocol_free_url(url_parts);
      return HYWB_ERR;
  }

  /* If we open a HTML page, we should set the base URL strings to something
   * apropriate.
   */
  if(headers->content_type_major && headers->content_type_minor)
  {
      if (!_stricmp(headers->content_type_major, "text")       //web or wap
          && (!_stricmp(headers->content_type_minor, "html") || !_stricmp(headers->content_type_minor, "vnd.wap.wml")))
      {
        protocol_store_base(headers->real_url);              
        if (headers->cookie)
          protocol_store_SID(base_host, headers->cookie);
      }
      else if (!_stricmp(headers->content_type_major, "application") && !_stricmp(headers->content_type_minor, "xml"))//rss
        protocol_store_base(headers->real_url);
  }

  FREEBUFFER(new_url,HYIE_MEM_IDX);
  protocol_free_url(url_parts);
  return fd;
}

/**
 * 获取数据流的指针，在数据流收发数据、关闭时得到数据流相关信息
 * eg：https或http、file等
 *
 * @param fd The file descriptor of the stream to close.
 *
 * @return non-zero value if an error occurred.
 */
struct protocol_stream *protocol_get_stream(int fd)
{
  struct protocol_stream *streamp, *previous_stream;

  /* Find the stream associated with the specified file descriptor. */
  previous_stream = first_stream;
  streamp = first_stream;
  while(streamp && streamp->fd != fd) {
    previous_stream = streamp;
    streamp = streamp->next;
  }

  if(streamp != NULL)
  {
  	previous_stream->next = streamp->next;
  }
  /* Maybe we cannot find the file descriptor in the list. What can we do
   * but to exit and tell the caller that there was an error. A thoughtful
   * alternative would be to use close() to close the unknown file
   * descriptor, but that is not really recommended.
   */
  return streamp;
}

/**
 * Closes a stream that was opened by protocol_stream_open(). It has 
 * to have been opened by that function, or it will not work. This
 * will free the memory that was allocated for the HTTP headers.
 *
 * @param fd The file descriptor of the stream to close.
 *
 * @return non-zero value if an error occurred.
 */
int protocol_close(int fd)
{

#if 1
	protocol_http_close(fd);
#else
  struct protocol_stream *streamp;
  
  if((streamp=protocol_get_stream(fd)) == NULL)
  {
  	//hyUsbPrintf("close socket fail  socket = %x\r\n",fd);
    return 1;
  }

//hyUsbPrintf("close socket protocol = %d\r\n",streamp->protocol);
  /* Close the stream, and remove it from the linked list. */
  switch(streamp->protocol) {
  case PROTOCOL_HTTP:
	protocol_http_close(fd);
    break;

  case PROTOCOL_FILE:
    //protocol_file_close(fd);
    break;
#ifdef HAVE_SSL
  case PROTOCOL_HTTPS:
	protocol_https_close(streamp->fd, streamp->ssl);
    streamp->ssl = NULL;
#endif    
  default:
	  break;
  }
  
  FREEBUFFER(streamp,HYIE_MEM_IDX);
  
#endif


///////////////////////////////////////////////////////////////////////////////////////////

  return HYWB_OK;
}
/**
*	when programe exit we free all the memory allocate by HTTP headers
*
*/
int free_httpHeader()
{
	struct protocol_stream *streamp, *previous_stream;
	
	streamp = first_stream;

	while(streamp) 
	{
		previous_stream = streamp;
		streamp = streamp->next;
		protocol_free_headers(previous_stream->headers);
		hyUsbPrintf("free_httpHeader headers = 0x%x    0x%x\r\n",(U32)previous_stream->headers, (U32)previous_stream);
		
		previous_stream->headers = NULL;
		FREEBUFFER(previous_stream,HYIE_MEM_IDX);
		
		hyUsbPrintf("free_httpHeader  streamp = 0x%x\r\n", (U32)streamp);
	}
    first_stream = NULL;
    return HYWB_OK;
}




/**
 * Get the HTTP headers associated with a particular stream.
 *
 * @param fd The file descriptor of the stream.
 * @param url the url of stream
 * @return a pointer to the struct holding the header information, or NULL
 * @return if the stream did not have any HTTP headers, or if the stream
 * @return could not be found for the given file descriptor.
 */
struct protocol_http_headers *protocol_get_headers(int fd,char *url)
{
  struct protocol_stream *streamp;

  streamp = first_stream;
  if (url == NULL)
  {
	  while(streamp && streamp->fd != fd) 
	  {
		streamp = streamp->next;
	  }
  }else
  {
  	  while(streamp && _stricmp(streamp->headers->real_url,url)) 
	  {
		streamp = streamp->next;
	  }
  }

  if(streamp == NULL)
    return NULL;

  return streamp->headers;
}

/**
 * Split up a URL into its different components. Basically, it tries
 * to use the fields in the URL struct logically, depending on the
 * different known protocol types. 
 *
 * @param orgurl The full URL, just as a piece of text.
 *
 * @return a pointer to an allocated URL struct, filled in with 
 * @return nice values taken from the given URL string. 
 */
struct protocol_url *protocol_split_url(char *orgurl)
{
    char *url, *urlp, *tmp;//, *tmp2;
    struct protocol_url *surl;
    
    surl = (struct protocol_url *)GETBUFFER(sizeof(struct protocol_url),HYIE_MEM_IDX);
    if(surl == NULL) {
        return NULL;
    }
    surl->type = NULL;
    surl->user = NULL;
    surl->pass = NULL;
    surl->host = NULL;
    surl->port = 0;
    surl->file = NULL;
    
    url = (char *)Strdup(orgurl);
    if(url == NULL)
        return NULL;
    
    urlp = url;
    tmp = (char *)strstr(urlp, "://");
    if(tmp != NULL) 
    {
        *tmp = '\0';
        tmp += 3;
        surl->type = (char *)Strdup(urlp);
        urlp = tmp;
    } 
    else if(!hy_memicmp(urlp, "mailto:", 7)) {
        surl->type = (char *)Strdup("mailto");
        urlp += 7;
    } 
    else 
    {
        surl->type = (char *)Strdup("file");
    }
    if(!_stricmp(surl->type, "file")) 
    {
        surl->file = (char *)Strdup(urlp);    
    } 
    else 
    {   /*tmp = strchr(urlp, '@');   //??ftp协议格式分析ftp://用户名@服务器域名/目录/文件,干扰正常http中带有@及:的Url分析
        if(tmp != NULL) {
            *tmp++ = '\0';
            tmp2 = tmp;
            tmp = strchr(urlp, ':');
            if(tmp != NULL) {
                *tmp++ = '\0';
                surl->pass = (char *)Strdup(tmp);
            }
            surl->user = (char *)Strdup(urlp);
            urlp = tmp2;
        }*/
        tmp = strchr(urlp, '/');
        if(tmp != NULL) 
        {
            *tmp++ = '\0';
            surl->file = (char *)Strdup(tmp);
        } 
        else 
        {
            surl->file = (char *)Strdup("");
        }
        tmp = strchr(urlp, ':');
        if(tmp != NULL) 
        {
            *tmp++ = '\0';
            surl->port = ATOI(tmp);
        }
        surl->host = (char *)Strdup(urlp);
    }
    
    if(surl->port == 0) 
    {
        surl->port = protocol_default_port(surl->type);
    }
    
    FREEBUFFER(url,HYIE_MEM_IDX);
    
    return surl;
}

/**
 * Takes a protocol string, and returns its default port
 * number.
 *
 * @param protocol The protocol name, e.g. "http", "ftp"
 *
 * @return the default port number of the protocol, or
 * @return zero if unknown
 */
unsigned int protocol_default_port(char *protocol)
{
/* These should really be taken from getservbyname(3), but it does not
 * use the exact same protocol names as a URL does. Later, dude.
 */
  if(!_stricmp(protocol, "http"))
    return 80;
  else if(!_stricmp(protocol, "https"))
    return 443;
  else if(!_stricmp(protocol, "ftp"))
    return 21;
  else if(!_stricmp(protocol, "mailto"))
    return 25;
  else if(!_stricmp(protocol, "telnet"))
    return 23;
  else if(!_stricmp(protocol, "gopher"))
    return 70;
  else
    return HYWB_OK;
}

/**
 * Takes a disassembled protocol_url struct, and puts it
 * into a URL string. This function is rather HTTP-centric,
 * and might need some care to work correctly with other
 * protocols.
 *
 * @param url The URL struct
 *
 * @return a pointer to an allocated URL string, or NULL if
 * @return malloc fails
 */
char *protocol_unsplit_url(struct protocol_url *url)
{
  int alloc_size;
  char *real_url;

  alloc_size = 7 + STRLEN(url->host) + STRLEN(url->file) + 64;
  if(url->user)
    alloc_size += STRLEN(url->user);
  if(url->pass)
    alloc_size += STRLEN(url->pass);
  real_url = (char *)GETBUFFER(alloc_size,HYIE_MEM_IDX);
  if(real_url == NULL) {
    return NULL;
  } else {
    strcpy(real_url, url->type);
    strcat(real_url, "://");
    if(url->user) {
      strcat(real_url, url->user);
      if(url->pass) {
	strcat(real_url, ":");
	strcat(real_url, url->pass);
      }
      strcat(real_url, "@");
    }
    strcat(real_url, url->host);
    if(url->port != protocol_default_port(url->type))
    {  
		int len;
		char string_buf[37]={0};
		len = STRLEN(real_url);
		*(real_url+len) = ':';
        strcpy(real_url+len+1, _itoa(url->port,string_buf,10));//j.w.li_1224 在非标准端口上传递数据需用十进制字符串标识eg:192.168.8.57:8000
       	//*(real_url+len+1) = url->port;
		//*(real_url+len+2) = 0;
	}
	if (url->file && _stricmp(url->file, ""))
    {
        strcat(real_url, "/");
        strcat(real_url, url->file);
    }
  }
  return real_url;
}


/**
 * 获取当前传输数据流的协议类型
 */
int protocol_get_type(int fd)
{ 
	struct protocol_http_headers *headers;
	int protocol = PROTOCOL_HTTP;

	// Fetch the HTTP headers associated with this stream. 
	headers = protocol_get_headers(fd,NULL);

	if(!hy_memicmp(headers->real_url, "http://", 7))
	{
		protocol = PROTOCOL_HTTP;
	}else if(!hy_memicmp(headers->real_url, "file://", 7))
	{
		protocol = PROTOCOL_FILE;
	}else if (!hy_memicmp(headers->real_url, "https://", 8))
	{
		protocol = PROTOCOL_HTTPS;
	}

	return protocol;
}


/**
 * Free a URL struct from all the evil allocated space.
 *
 * @param url A pointer to a URL struct.
 */
void protocol_free_url(struct protocol_url *url)
{
  if(url) {
    if(url->type)
      FREEBUFFER(url->type,HYIE_MEM_IDX);
    if(url->user)
      FREEBUFFER(url->user,HYIE_MEM_IDX);
    if(url->pass)
      FREEBUFFER(url->pass,HYIE_MEM_IDX);
    if(url->host)
      FREEBUFFER(url->host,HYIE_MEM_IDX);
    if(url->file)
      FREEBUFFER(url->file,HYIE_MEM_IDX);

    FREEBUFFER(url,HYIE_MEM_IDX);
  }
}

/**
 * 接收数据，根据socket的descriptor来确定接收数据的方式:
 * https的ssl接收或标准的socket的recv
 *
 * @param fd The socket descriptor
 */
int protocol_recv(int type, void *handle, char *buf, int len)
{
	int ret, errCnt=0;
	int fd;
	struct timeval to;   
    fd_set fs;
	
	if (type == PROTOCOL_HTTP)
	{
		fd = *(int *)handle;
		
		to.tv_sec  = 5;   
	    to.tv_usec = 0;
	    FD_ZERO(&fs);   
	    FD_SET(fd, &fs);
	    
		while (1)
		{
			if(Net_GsmReset() == 1)
			{
				return -1;
			}
			
			ret = select(fd, &fs, 0, 0, &to);
			if (ret > 0)
			{
				return recv(fd, buf, len, 0);
			}
			else if (ret == 0)
			{
				errCnt++;
				//hyUsbPrintf("----recv timeout-----%d\r\n",errCnt);
				sleep(1);
				if (errCnt>=5)
			    {
			        return HYWB_ERR;
			    }
			}
			else
			{
				return HYWB_ERR;
			}
		}
	}
    else if (type == PROTOCOL_FILE)
    {
        ret = AbstractFileSystem_Read(*(int *)handle, buf, len);
	    return ret;
    }

#ifdef HAVE_SSL
	else if (type == PROTOCOL_HTTPS)
	{
		SSL *ssl;
		ssl = (SSL *)handle;
		return SSL_read(ssl, buf, len);
	}
#endif
	else 
		return HYWB_ERR;
}

/**
 * 发送数据，根据socket的descriptor来确定接收数据的方式:
 * https的ssl接收或标准的socket的send
 *
 * @param fd The socket descriptor
 */
int protocol_send(int type, void *handle, char *buf, int len)
{
	if (type == PROTOCOL_HTTP)
	{
		int fd;
		fd = *(int *)handle;
		return send(fd, buf, len, 0);
	}
#ifdef HAVE_SSL
	else if (type == PROTOCOL_HTTPS)
	{
		SSL *ssl;
		ssl = (SSL *)handle;
		return SSL_write(ssl, buf, len);
	}
#endif
	else 
		return HYWB_ERR;
}

/**
 * process JSESSIONID
 */
int protocol_search_SID(char *host)
{
    int i;
    
    for (i=0; i<session_cnt; i++)
    {
        if (!strcasecmp(session[i].host+7, host) || !strcasecmp(session[i].host, host))
            return i;
    }
    return HYWB_ERR;
}

char *protocol_get_SID(int id_num)
{
    if (id_num >= 0 && id_num < session_cnt)
        return session[id_num].id;
    else
        return NULL;
}

int protocol_store_SID(char *host, char *id)
{
    int idNum;

    if (host==NULL || id==NULL)
        return HYWB_ERR;

    if (session_cnt > 10)    
        return HYWB_ERR;

    if ((idNum = protocol_search_SID(host)) != HYWB_ERR)
    {    
        FREEBUFFER(session[idNum].id,HYIE_MEM_IDX);
        session[idNum].id = (char *)GETBUFFER(strlen(id)+1,HYIE_MEM_IDX);
        if (session[idNum].id == NULL)
            return HYWB_ERR;
        
        strcpy(session[idNum].id, id);
        return HYWB_OK;
    }

    session[session_cnt].host = (char *)GETBUFFER(strlen(host)+1,HYIE_MEM_IDX);
    if (session[session_cnt].host == NULL)
        return HYWB_ERR;

    session[session_cnt].id = (char *)GETBUFFER(strlen(id)+1,HYIE_MEM_IDX);
    if (session[session_cnt].id == NULL)
        return HYWB_ERR;

    strcpy(session[session_cnt].host, host);
    strcpy(session[session_cnt].id, id);
    session_cnt ++;
    return HYWB_OK;
}

void protocol_destroy_SID(void)
{
    int i;

    for (i=0; i<session_cnt; i++)
    {
        FREEBUFFER(session[i].host,HYIE_MEM_IDX);
        FREEBUFFER(session[i].id,HYIE_MEM_IDX);
    }
}