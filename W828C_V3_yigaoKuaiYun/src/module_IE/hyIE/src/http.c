/**
 * Functions to open a stream from a server using the HTTP protocol.
 * This currently only supports HTTP/1.0 but that is enough for most
 * things.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#include "http.h"
#include "https.h"


#include "socket_api.h"
#include "string.h"

#ifndef NO_WEB_DEBUG

#ifdef HAVE_SSL
    SSL *pStream_ssl;          //connect时保存ssl descriptor,成功后保存到stream->ssl,全局控制
#endif
#define TCP_TIMEOUT   40000    //超时40s

char *proxy_url = NULL;

int  gIELastErr;


int IE_GetLastErr()
{
	return gIELastErr;
}


/**
 * 设置浏览器的设置参数
 *
 * @param name  IN  设置参数名称
 * @param value IN  设置参数值 
 *
 * @return -1 if error occurs
 */
int protocol_setting_set(char *name, char *value)
{
	proxy_url = value;
	return HYWB_OK;
}


/**
 * 获取浏览器的设置参数
 *
 * @param name  IN  设置参数名称
 * @param value OUT 设置参数值 
 *
 * @return -1 if error occurs
 */
int protocol_setting_get(char *name, char **value)
{
	*value = proxy_url;
	return HYWB_OK;
}

/**
 * 处理网络连接时的select
 *
 * @param sock SOCKET  
 * @param sec  timeout seconds  
 * @param sec  timeout microseconds  微妙
 * @param type select status  
 *
 * @return a file descriptor for the opened stream, or a negative value
 * @return representing an error.
 */
#define   READ_STATUS           0
#define   WRITE_STATUS          1
#define   EXCPT_STATUS          2
int protocol_http_select(int sock, int sec, int usec, short type)
{   
    int st;   
    struct timeval to;   
    fd_set fs;   
        
    to.tv_sec = sec;   
    to.tv_usec = usec;   
        
    FD_ZERO(&fs);   
    FD_SET(sock, &fs);   

    switch(type)
    {   
        case READ_STATUS:   
            st = select(sock, &fs, 0, 0, &to);   
            break;   
            
        case WRITE_STATUS:   
            st = select(sock, 0, &fs, 0, &to);   
            break;   
            
        case EXCPT_STATUS:   
            st = select(sock, 0, 0, &fs, &to);   
            break;   
    }   
    return(st);   
}
  
U32 gu32IE_ConnectFail = 0;

U32 Ie_get_connectfailcnt()
{
	return gu32IE_ConnectFail;
}

void Ie_clear_connectfailcnt()
{
	gu32IE_ConnectFail = 0;
}

WebHostBak gtIEDns;

void hyIE_clearDns()
{
	memset((char *)&gtIEDns,0,sizeof(WebHostBak));
}

//0:成功  -1:失败
int Dns_note_hosts_new(WebHostBak *kDns, const char *hostname)
{
	struct hostent *hent;
	int state;

	hent = gethostbyname_noblock(hostname);
	while (1)
	{
		state = checkhostdata(hent);  //>0正确 =0错误或超时 <0正在处理
		if (state > 0)
			break;
		sleep(2);
		if (Net_GsmReset() == 1 || state < 0)
			return -1;
	}

	kDns->alen = hent->h_length;
	
	memcpy(kDns->data, hent->h_addr_list[0], hent->h_length);
	kDns->data[hent->h_length] = 0;
	
	return 0;
}


/**
 * Open a TCP connection to the other host, on a specific port number. 
 *
 * @param url A pointer to a URL struct containing the hostname and the
 * @param url port number to use for the connection.
 *
 * @return a file descriptor for the opened stream, or a negative value
 * @return representing an error.
 */
int protocol_http_open_connection(struct protocol_url *url)
{
    struct sockaddr_in saddr;
    WebHostBak twh;
    int sock;
    int errRet;
    unsigned long tmout;
    
#ifdef PLATFORM_PC
    struct protoent *proto;
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(1, 1);
#endif

    if (url->host == NULL)
    {
    //hyUsbPrintf("host fail === %s\r\n",url->host);
        return -1;
    }

    
#ifdef PLATFORM_PC
    errort = WSAStartup( wVersionRequested, &wsaData );
    proto = getprotobyname("tcp");
    errort = WSAGetLastError();
    
    if(proto == NULL) 
        return -1;
    sock = socket(AF_INET, SOCK_STREAM, proto->p_proto);
#else
    sock = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if(sock < 0) 
    {
    hyUsbPrintf("socket fail === %d\r\n",sock);
        return -1;
    }

	//DNS solving and cache.....
    /*if(gtIEDns.alen > 0 && strlen(gtIEDns.data) > 0)
    {
    	//hyUsbPrintf("use old dns ======\r\n");
    	memcpy(&saddr.sin_addr,gtIEDns.data, gtIEDns.alen);
    }
    else*/
    {
    	//hyUsbPrintf("use new dns ======\r\n");
    	if(-1 == Dns_note_hosts_new(&twh,url->host))
    	{
    		protocol_http_close(sock);//dns 失败
	        gu32IE_ConnectFail++;
	        hyUsbPrintf("dns fail = %d!\r\n",gu32IE_ConnectFail);
	        return -1;
    	}
    	else
    	{
    		gu32IE_ConnectFail = 0;
    	}

	    gtIEDns.alen = twh.alen;
	    memcpy(gtIEDns.data,twh.data, twh.alen);
	    gtIEDns.data[twh.alen] = 0;
	    
	    memcpy(&saddr.sin_addr,twh.data, twh.alen);
    }
    //PhoneTrace(0, "Get host name OK");   
    
    tmout = TCP_TIMEOUT;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tmout, sizeof(tmout)); 
    ioctlsocket(sock, FIONBIO, 1);
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons((unsigned short)url->port);
        
    errRet = connect(sock, (struct sockaddr *)&saddr, sizeof(saddr));

    if(errRet < 0) 
    {
        protocol_http_close(sock);//connect 失败
        gu32IE_ConnectFail++;
        //清空dns
		hyIE_clearDns();
        hyUsbPrintf("connect fail  = %d!\r\n",gu32IE_ConnectFail);
        return -1;
    }
    else
    {
    	gu32IE_ConnectFail = 0;
    }
//hyUsbPrintf("socket ret === %d\r\n",sock);  
    return sock;
}


/**
 * Format an HTTP request to a webserver on an open stream . 
 * POST or GET method is difference
 *
 * @param url A pointer to a URL struct containing the things needed
 * @param url to make a request.
 * @param use_proxy A non-zero value if a proxy is used.
 * @param referer The referring URL which was used to get to the currently
 * @param referer requested page.
 *
 * @return non-zero value if an error occurred.
 */
//extern unsigned char gu8Gzip;
//extern unsigned char gu8Alive;
char *protocol_format_request(struct protocol_url *url, int use_proxy, struct protocol_http_request *request_headers, int *sendlen) 
{
	char *referer_text=NULL, *tmp=NULL, *request=NULL, *auth_text=NULL, *user_agent=NULL;
    char *uri=NULL, *sid;
    int  len,allocSize,appendH=0,appendT=0;
	
	*sendlen  = 0;
	allocSize = 0xC800;//0xF000;
	
	/* Create header text for a referer header, if referer is given. */
	if(request_headers->referer != NULL) 
	{
		referer_text = (char *)GETBUFFER(STRLEN(request_headers->referer) + 16,HYIE_MEM_IDX);
		if(referer_text == NULL) 
		{
			return NULL;
		}
		strcpy(referer_text, "Referer: ");
		strcat(referer_text, request_headers->referer);
		strcat(referer_text, "\r\n");
	} 
	else 
		referer_text = "";
	 
	/* Create header text for authorization, if user and pass are given. */
	if(url->user != NULL && url->pass != NULL) 
	{
		auth_text = (char *)GETBUFFER((STRLEN(url->user) + STRLEN(url->pass)) * 2 + 32,HYIE_MEM_IDX);
		if(auth_text == NULL) 
			 return NULL;

		tmp = (char *)GETBUFFER((STRLEN(url->user) + STRLEN(url->pass)) * 2 + 32,HYIE_MEM_IDX);
		if(tmp == NULL) 
		{
			FREEBUFFER(auth_text,HYIE_MEM_IDX);
			return NULL;
		}
		strcpy(auth_text, url->user);
		strcat(auth_text, ":");
		strcat(auth_text, url->pass);
		mbase64_encode(auth_text, STRLEN(auth_text), tmp);
		strcpy(auth_text, "Authorization: Basic ");
		strcat(auth_text, tmp);
		strcat(auth_text, "\r\n");
		FREEBUFFER(tmp,HYIE_MEM_IDX);
	} 
	else 
		auth_text = "";
	 
	/* Let us see what User-Agent we should disguise ourselves as today. */
	//user_agent = (char *)GETBUFFER(1024);
	user_agent = (char *)GETBUFFER(256,HYIE_MEM_IDX);
	if(user_agent == NULL)
		return NULL;
	
	//tmp = "Opera/9.24 (Windows NT 5.1; U; zh-cn)";            //opera浏览器的服务器代理
    if	( request_headers->user_agent == NULL )
    {
        strcpy(user_agent, "Opera/9.24 (Windows NT 5.1; U; zh-cn)");
    }
    else
    {
        strcpy(user_agent, request_headers->user_agent);
    }
    
    //strncpy(user_agent, tmp, 1023);
    //user_agent[1023] = '\0';
	 
	/* Create the HTTP request to retreive an object from the server. */
	request = (char *)GETBUFFER(allocSize,HYIE_MEM_IDX); /*0804 分配60 KB */
	if(request == NULL) 
	{
		FREEBUFFER(user_agent,HYIE_MEM_IDX);
		return NULL;
	}

	uri = Strdup(url->file) ;
	
    sid = protocol_get_SID(protocol_search_SID(url->host));
	
	if (request_headers->method == HTTP_GET_METHOD)
	{
        if (use_proxy)
        {
            strcpy(request, "GET ");
            strcat(request, url->type);
            strcat(request, "://");
            strcat(request, url->host);
            strcat(request, ":");
            _itoa(url->port, request+STRLEN(request), 10);
            strcat(request, "/");
            strcat(request, uri);
		}
        else
        {
            strcpy(request, "GET /");
            strcat(request, uri);
        }
        strcat(request, " HTTP/1.1\r\nUser-Agent: ");
		strcat(request, user_agent);
        strcat(request, "\r\nHost: ");
		strcat(request, url->host);
		strcat(request, ":");
        _itoa(url->port, request+STRLEN(request), 10);
        if (request_headers->range != NULL)     //断点续传
        {   
            strcat(request, "\r\nRANGE: bytes=");
            strcat(request, request_headers->range);
            //strcat(request, "-");
        }
		strcat(request, "\r\nAccept: */*\r\n");
		
		if(request_headers->gzip == 1)//需要gzip压缩   http 1.1才支持压缩
        {
        	strcat(request, "Accept-Encoding: deflate, gzip\r\n");
        }
        
        if(request_headers->connection == 1)
        	strcat(request, "Connection: Keep-Alive\r\n");//第7行:连接设置,保持
		else
			strcat(request, "Connection: close\r\n");//第7行:连接设置,关闭   kong
		
		strcat(request, referer_text);
		strcat(request, auth_text);
#ifdef HAVE_COOKIES
        if (cookie != NULL)
        {
            strcat(request, "\r\nCookie2: $Version=\"1\"\r\n");
            strcat(request, cookie);
        }
#endif
		len = strlen(request);
        if (request_headers->content_length > 0 && request_headers->content_data != NULL)   //扩展字段
        {
        	strcat(request, "\r\n");
			if(len + request_headers->content_length < allocSize)
            {
	            memcpy(request, request_headers->content_data, request_headers->content_length);
	            len += request_headers->content_length;
	        }
        }
        else
        {
        	strcat(request, "\r\n");
        	len += 2;
        }
        
        *sendlen = len;
	}
	else if (request_headers->method == HTTP_POST_METHOD)
	{
        char string_buf[37]={0};
        
        strcpy(request, "POST /");			//第1行:方法,请求的路径,版本
        strcat(request, uri);
        
		strcat(request, " HTTP/1.1\r\n");
		strcat(request, "Accept: */*\r\n");	//第2行:接收的数据类型
		if(request_headers->gzip == 1)//需要gzip压缩   http 1.1才支持压缩
        {
        	strcat(request, "Accept-Encoding: deflate, gzip\r\n");
        }

        strcat(request, referer_text);		//第3行:Referer
		strcat(request, "Content-Type: ");	//第4行:Content-Type
        strcat(request, request_headers->content_type);
		if (request_headers->content_type == NULL)
        {
            return NULL;
        }
        else if (!_stricmp(request_headers->content_type, "multipart/form-data;"))//上传
        {
        	strcat(request, " boundary=---------------------------7d81b715025a");
       	
        	appendH += strlen("-----------------------------7d81b715025a\r\nContent-Disposition: ");
        	appendH += strlen(request_headers->Content_Disposition);
        	appendH += strlen("\r\n\r\n");
        	appendT = strlen("\r\n-----------------------------7d81b715025a--");
        }
		strcat(request, "\r\nUser-Agent: ");	//第5行:浏览器类型
		strcat(request, user_agent);
		strcat(request, "\r\nHost: ");		//第6行:主机
		strcat(request, url->host);
		sprintf(string_buf, ":%d", url->port);
		strcat(request, string_buf);//端口
		
		strcat(request, "\r\n");
        if(request_headers->connection == 1)
        	strcat(request, "Connection: Keep-Alive\r\n");//第7行:连接设置,保持
		else
			strcat(request, "Connection: close\r\n");//第7行:连接设置,关闭   kong
   		
   		strcat(request, "Content-Length: ");//第8行:Content-Length
        strcat(request, _itoa(request_headers->content_length+appendH+appendT,string_buf,10));  
        strcat(request, "\r\n\r\n");        //结束的回车换行
        
        len = strlen(request);
        if (request_headers->content_length > 0 && request_headers->content_data != NULL)
        {
        	if(len + request_headers->content_length+appendH+appendT < allocSize)
            {
            	if(appendH > 0)
            	{
            		sprintf(request+len, "-----------------------------7d81b715025a\r\nContent-Disposition: %s\r\n\r\n", request_headers->Content_Disposition);
            		len += appendH;
            	}
	            memcpy(request+len, request_headers->content_data, request_headers->content_length);
	            len += request_headers->content_length;
	            if(appendT > 0)
            	{
            		strcpy(request+len, "\r\n-----------------------------7d81b715025a--");
            		len += appendT;
            	}
	        }
		}
		
		*sendlen = len;
	}
	 
#ifdef WB_DEBUG
	fprintf(stderr, "Request:\n%s\n", request);
#endif /* DEBUG */
	if(STRLEN(referer_text) > 0)
		FREEBUFFER(referer_text,HYIE_MEM_IDX);
    if (uri != NULL)
        FREEBUFFER(uri,HYIE_MEM_IDX);
	FREEBUFFER(user_agent,HYIE_MEM_IDX);
	return request;
}


/**
 * Take care of the response from the server. 
 * Fill in the header struct with appropriate values.
 * This also have to take into account stupidly implemented HTTP servers
 * which send only LF instead of CRLF as the RFC clearly specifies.
 *
 * @param url A pointer to a URL struct containing the things needed
 * @param url to make a request.
 * @param use_proxy A non-zero value if a proxy is used.
 * @param referer The referring URL which was used to get to the currently
 * @param referer requested page.
 *
 * @return non-zero value if an error occurred.
 */
int protocol_http_get_responseLine(int fd,
    struct protocol_http_headers *headers)
{
	int error, i, type;
	char *tmp;
	char *header_row;
	char c;
	void *handle;

	error = 0;
	header_row = (char *)GETBUFFER(1024,HYIE_MEM_IDX);
	if(header_row == NULL) 
		return -1;
	 
	/* First read the response code. */
	headers->return_code = 20000;//404;
#ifdef HAVE_SSL
	if (pStream_ssl != NULL)
	{	type = PROTOCOL_HTTPS;
		handle = pStream_ssl;
	}
	else
#endif
	{	type = PROTOCOL_HTTP;
		handle = &fd;
	}
hyUsbPrintf("responseLine free 0x%x \r\n", (U32)headers->location);
    FREEBUFFER(headers->location,HYIE_MEM_IDX);    //释放在302时占用的内存空间
    FREEBUFFER(headers->server,HYIE_MEM_IDX);
    FREEBUFFER(headers->content_type_major,HYIE_MEM_IDX);
    FREEBUFFER(headers->content_type_minor,HYIE_MEM_IDX);
    FREEBUFFER(headers->charset,HYIE_MEM_IDX);
    FREEBUFFER(headers->return_message,HYIE_MEM_IDX);
    FREEBUFFER(headers->cookie,HYIE_MEM_IDX);
	headers->content_length = 0;
hyUsbPrintf("111 responseLine free 0x%x \r\n", (U32)headers->location);	
	if(protocol_recv(type, handle, header_row, 9) > 0) 
	{
		header_row[9] = '\0';
		if(!hy_memicmp(header_row, "HTTP/", 5) && protocol_recv(type, handle, header_row, 4) > 0)
		{
			headers->return_code = ATOI(header_row);
			i = 0;
			while(protocol_recv(type, handle, &c, 1) > 0 && c != '\r' && c != '\n') 
			{
				if(i < 1023)
					header_row[i++] = c;
			}
			if(c == '\r')
				protocol_recv(type, handle, &c, 1);

			if(i > 0) 
			{
				header_row[i] = '\0';
				tmp = (char *)GETBUFFER(STRLEN(header_row) + 1,HYIE_MEM_IDX);
				if(tmp == NULL)
				{
					error = 1;
				}
				else
				{
					strcpy(tmp, header_row);
					headers->return_message = tmp;      
				}
			}
		}
	}
	else
	{
		hyUsbPrintf("ie recv nothing \r\n");
		error = 1;
	}
	 
	/* Read all headers and save the valuable ones. 
	 * Again, make sure unproper HTTP servers also gets treated.
	 */
	i = 0;
	while(!error && protocol_recv(type, handle, &c, 1) > 0) 
	{
		if(c == '\r' || c == '\n') 
		{
			char *field, *value;
			 
			// If we come to a CR, always expect an LF. 
			if(c == '\r')
				protocol_recv(type, handle, &c, 1);
			 
			// If no characters have been read since previous row, it means the
			// actual stream data is coming next, right after another line feed.
			 
			if(i == 0 && c == '\n')
				 break;
			 
			header_row[i] = '\0';
			i = 0;
			field = header_row;
			value = strstr(header_row, ":");
			if(value == NULL)
				continue;
			value[0] = '\0';

            //skip the space(' ')
            value ++;
            while (*value == ' ')
                value ++;    

			// Figure out where to put the data. 
			if(!strcasecmp(field, "Content-Type"))
			{
				char *divider;
				 
				tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
				if(tmp == NULL) {
					error = 1;
					continue;
				}
				strcpy(tmp, value);
				headers->content_type_major = tmp;
				 
				divider = strchr(tmp, '/');
				if(divider == NULL)
					continue;
				divider[0] = '\0';
				tmp = (char *)GETBUFFER(STRLEN(&divider[1]) + 1,HYIE_MEM_IDX);
				if(tmp == NULL) {
					error = 1;
					continue;
				}
				strcpy(tmp, &divider[1]);
				headers->content_type_minor = tmp;
				 
				// Look for other options for the content type. This is most
				// likely the charset. Currently, we do not use the charset
				// value for anything, but always assume ISO-8859-1 throughout
				// the whole program. Think what you like about that. 
				 
				// Note that I have done this simple, and it is not the charset
				// which is placed in headers->charset, but the whole option
				// string. This will change, when there is a need to use the
				// charset value, or when I have nothing better to do, whichever
				// comes first.
				 
				divider = strchr(tmp, ';');
				if(divider == NULL)
					continue;
				divider[0] = '\0';
				tmp = (char *)GETBUFFER(STRLEN(&divider[1]) + 1,HYIE_MEM_IDX);
				if(tmp == NULL) 
				{
					error = 1;
					continue;
				}
				strcpy(tmp, &divider[1]);
			    headers->charset = (char *)GETBUFFER(strlen(tmp)+1,HYIE_MEM_IDX);
                if (headers->charset != NULL)
                    strcpy(headers->charset, strrchr(tmp, '=')+1);
				FREEBUFFER(tmp,HYIE_MEM_IDX);
			} 
			else if(!strcasecmp(field, "Server")) 
			{
				tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
				if(tmp == NULL) 
				{
					error = 1;
					continue;
				}
				strcpy(tmp, value);
				headers->server = tmp;
			} 
			else if(!strcasecmp(field, "Location")) 
			{
				tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
				if(tmp == NULL) 
				{
					error = 1;
					continue;
				}
				strcpy(tmp, value);
				headers->location = tmp;
			}
			else if(!strcasecmp(field, "Content-Length")) 
			{
				headers->content_length = atol(value);
			}
			else if(!strcasecmp(field, "Content-Encoding"))
            {
                tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
                if (tmp == NULL)
                {
                    error = 1;
                    continue;
                }
                strcpy(tmp, value);
                headers->content_encoding = tmp;
            }
            else if(!strcasecmp(field, "connection"))
            {
            	tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
                if (tmp == NULL)
                {
                    error = 1;
                    continue;
                }
                strcpy(tmp, value);
                headers->connection = tmp;
            }
            else if (!strcasecmp(field, "Set-Cookie"))
            {
            	char *id;
                if((id = strstr(value, "JSESSIONID")) != NULL)
                {
	                char *semicolon;
	                int len;

	                headers->cookie = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
	                if (headers->cookie == NULL)
	                {
	                	error = 1;
						continue;
					}
	                
                    semicolon = strstr(id, ";");
                    len = strlen("JSESSIONID=");
                    strncpy(headers->cookie, id+len, semicolon-id-len);
                    headers->cookie[semicolon-id-len] = '\0'; 
                }
            }
            else if(!strcasecmp(field, "Transfer-Encoding"))
			{
				tmp = (char *)GETBUFFER(STRLEN(value) + 1,HYIE_MEM_IDX);
                if (tmp == NULL)
                {
                    error = 1;
                    continue;
                }
                strcpy(tmp, value);
                headers->TransferEncoding = tmp;
			}
		} 
		else 
		{
			if(c != '\n' && i < 1023)
				header_row[i++] = c;
		}
	}
  
	FREEBUFFER(header_row,HYIE_MEM_IDX);
	return error;
}
/**
 * Make an HTTP request to a webserver on an open stream. This reads
 * the important headers from the response and fills in the struct
 * available for this purpose.
 *
 * @param fd The file descriptor for the stream to request on.
 * @param url A pointer to a URL struct containing the things needed
 * @param url to make a request.
 * @param use_proxy A non-zero value if a proxy is used.
 * @param referer The referring URL which was used to get to the currently
 * @param referer requested page.
 * @param headers A pointer to an HTTP headers struct, which will be
 * @param headers filled in with appropriate values, taken from the
 * @param headers response from the server.
 *
 * @return non-zero value if an error occurred.
 */
static int protocol_http_make_request(int fd, struct protocol_url *url,
	int use_proxy, 
	struct protocol_http_headers *headers,
	struct protocol_http_request *request_headers)
{
	int error, type;
	char *request;
	void *handle;
	int sendLen;

	error = 0; 
	type = PROTOCOL_HTTP;
	handle = &fd;
	
	
	//组织http请求数据头 
	sendLen = 0;
	request = protocol_format_request(url, use_proxy, request_headers, &sendLen);

#ifdef HAVE_SSL
	pStream_ssl = NULL;
	if (!strcasecmp(url->type, "https"))
	{	if ((pStream_ssl=protocol_https_connect(fd)) == NULL)
			return -1;
		type = PROTOCOL_HTTPS;
		handle = pStream_ssl;
	}
#endif
	//发送请求
	if (request != NULL && sendLen > 0)
	{
		//hyUsbPrintf("protocol_send =======\r\n");
		//hyUsbMessageByLen(request,sendLen);
		//hyUsbPrintf("\r\n");
		if(protocol_send(type, handle, request, sendLen)==-1)
		{	
			gIELastErr = -1;
			FREEBUFFER(request,HYIE_MEM_IDX);
			hyUsbPrintf("protocol_send   fail\r\n");
			return -1;
		}
        FREEBUFFER(request,HYIE_MEM_IDX);
        //hyUsbPrintf("protocol_send   ok\r\n");
	}
	else
	{
		gIELastErr = -1;
		hyUsbPrintf("request NULL   len = %d \r\n", sendLen);
		return -1;
	}
	//获取请求回应，并进行处理
//	hyUsbPrintf("Prepare to get responseLine\r\n");
	error = protocol_http_get_responseLine(fd, headers);
	hyUsbPrintf("succ to get responseLine=%d\r\n", headers->return_code);

	//!!!POST方式提交表单时,可能连续发送大量数据成功后,才会回应,但可能已超过超时时间
	//暂处理为不等待回应,继续发送数据
    if (headers->content_type_major==NULL && headers->content_type_minor==NULL 
     && request_headers->content_type != NULL
     && !_stricmp(request_headers->content_type, "multipart/form-data;"))
        headers->return_code = 200;
    
    gIELastErr = headers->return_code;
    
	return error;
}

/**
 * Deal with the return code as we see fit. 
 * Fill in more return codes when we find out what they really stand for.
 *
 * @param url The name of the URL to open.
 * @param referer The URL we were at when entering this new URL.
 * @param headers A pointer to a struct meant to contain information gathered
 * @param headers from the HTTP headers in the response from the server. If
 * @param headers this is set to NULL, do not even try to store any headers.
 *
 * @return the file descriptor for the http stream or a negative value
 * @return if an error occurred.
 */
int protocol_http_deal_response(int fd, int using_proxy, struct protocol_url *url, 
						struct protocol_http_headers *headers,
						struct protocol_http_request *request_headers)
{
    struct protocol_url *proxy_url = NULL, *relocation_url;
	char *tmp, *absolute_relocation;
	int sock=fd;
//hyUsbPrintf("return code === %d\r\n",headers->return_code);
	//PhoneTrace(0, "return code = %d", headers->return_code);
	switch(headers->return_code) 
	{
	/* Not found. */
	case 404:
	/* If it could not find the page, try adding an ending slash, to get it as
	 * a directory. Only do this, if the file did not have an ending slash 
	 * already.
	 */
#if 0
	if(url->file && url->file[STRLEN(url->file) - 1] != '/') 
	{
		 /* Close the old stream. */
	    protocol_http_close(sock);
	  
	    /* Open it again. */
	    if(using_proxy)
	    {
	    	char *value = NULL;
	    	protocol_setting_get(NULL, &value);
	    	proxy_url = protocol_split_url((char *)value);
	    	sock = protocol_http_open_connection(proxy_url);
	    	protocol_free_url(proxy_url);
	    }
	    else
	    	sock = protocol_http_open_connection(url);
	    if(sock < 0)
			break;

	    tmp = (char *)GETBUFFER(STRLEN(url->file) + 2);
	  	if(tmp == NULL) 
	  	{
			sock = -1;
			break;
	  	}
	    strcpy(tmp, url->file);
	  	strcat(tmp, "/");
	  	FREEBUFFER(url->file);
	  	url->file = tmp;

	  	protocol_http_make_request(sock, url, using_proxy, headers, request_headers);

	  	/* There is the possibility here that the new return code is not 404 or
	   	 * 200. However, we ignore that possibility now, and only accept 200 as
	     * being ok.
	     */
	  	if(headers->return_code != 200 && headers->return_code != 206) 
	  	{
			protocol_http_close(sock);
			sock = -1;
			break;
	  	}
	}
	else 
#endif
	{
		protocol_http_close(sock);//没有请求资源
	  	sock = -1;
	  	break;
	}

	/* Last break is deliberately missing, because if we get here, the new
	 * return code was 200, and then we should go on as usual.
	 */

	case 100:/* Continue */
	case 206:/* 局部内容,用于断点续传*/
	case 200:/* OK. */
	/* Store the complete URL used for reading. */
	headers->real_url = protocol_unsplit_url(url);
	break;

	/* Relocation. */
	case 301:
	case 302:
	case 307: /* 临时重定向,暂同301,但在浏览历史中不应缓存uri,仍应访问定向前的地址后重定向,因重定向地址会变 */
		/* If we get codes 301 or 302, it means the page has moved permantently
	   	 * or temporarily. Either way, we recursively call protocol_http_open
	 	 * again, with the new location.
	 	 * Later, we will have to check for endless loops of these. We cannot 
	 	 * allow evil system administrators to crash the program.
	 	 */
		/* Close the old stream. */
		protocol_http_close(sock);

		/* Set the status to something informative. */
		tmp = (char *)GETBUFFER(STRLEN(headers->location) + 64,HYIE_MEM_IDX);
		if(tmp == NULL)
	  		break;
		strcpy(tmp, /*gu8IE_StrInfor[IE_RELOCATED][0]*/" ");
		strcat(tmp, headers->location);
		//a_interface_set_status(tmp);
		FREEBUFFER(tmp,HYIE_MEM_IDX);

		/* According to the HTTP standard, the Location must be
	 	 * absolute, starting with "http://". But what if it isn't?
	 	 * The following code assumes that if Location is relative,
	 	 * it is relative to the URL that returned 30x.
	 	 */
	 	hyUsbPrintf("url = %s \r\n", url);
		tmp = protocol_unsplit_url(url);//重定向的location为绝对地址则不要保存host,相对地址则需要保存host
		if(!hy_memicmp(headers->location, "http://", 7) || !hy_memicmp(headers->location, "https://", 8))
	    	absolute_relocation = protocol_make_absolute(headers->location, NULL);
		else
	    	absolute_relocation = protocol_make_absolute(headers->location, tmp); 
		relocation_url = protocol_split_url(absolute_relocation);
		FREEBUFFER(absolute_relocation,HYIE_MEM_IDX);
		FREEBUFFER(tmp,HYIE_MEM_IDX);
		if(relocation_url)
	  	/* What should be in the Referer field: the original referer,
	     * or the URL that returned 30x? Well, NULL is never wrong...
	     */
		  	sock = protocol_http_open(relocation_url, headers, request_headers);    
		else
	  		sock = -1;
		protocol_free_url(relocation_url);
		break;

		/* Since we do not know what to do at this point, we just say that
	 	 * the page was not found, and let the program act as if it was not.
	 	 */
	default:
		#ifdef WB_DEBUG
		fprintf(stderr, "Unknown return code %d at http://%s/%s port %d.\n",
		    headers->return_code, url->host, url->file, url->port);
		#endif
		protocol_http_close(sock);//错误响应
		headers->return_code = 404;
		sock = -1;
	}
	return sock;
}

int protocol_send_request(int sock, char *url, struct protocol_http_request *request,struct protocol_http_headers *headers)
{
	char *new_url;
	struct protocol_url *url_parts=NULL;
	
	gIELastErr = 20000;
	
	/* Get the absolute equivalence to the specified URL. */
 	new_url = protocol_make_absolute(url, NULL);
  	
  	if(new_url)
  	{
  		url_parts = protocol_split_url(new_url);//分析url地址的组成
  		if(url_parts)
  		{
  			
  		}
  		else
	  	{
	  		FREEBUFFER(new_url,HYIE_MEM_IDX);

			gIELastErr = -1;
	  		return -1;
	  	}
  	}
  	else
  	{
  		gIELastErr = -1;
  		return -1;
  	}
	
	if (protocol_http_make_request(sock, url_parts, 0, headers, request) < 0)
	{
		FREEBUFFER(new_url,HYIE_MEM_IDX);
  		protocol_free_url(url_parts);
		return -1;
	}
	
	//hyUsbPrintf("return code = %d\r\n",headers->return_code);
	if(headers->return_code == 200 || 
	   headers->return_code == 100 || 
	   headers->return_code == 206)
	{
		headers->real_url = protocol_unsplit_url(url_parts);
	}
	else
	{
		headers->return_code = 404;
	}
	
	/* This is not really right. */
	if(headers->return_code != 200 && headers->return_code != 206) 
	{
		if(headers->return_code == 100)//释放资源，处理上传(如此处理，权益之计，对于网络方面需要调整的地方较多)
		{	
			FREEBUFFER(new_url,HYIE_MEM_IDX);
  			protocol_free_url(url_parts);
			return -1;
		}

		FREEBUFFER(new_url,HYIE_MEM_IDX);
  		protocol_free_url(url_parts);
		return -1;
	}

	if (protocol_add_stream(sock, PROTOCOL_HTTP, headers) == HYWB_ERR)
	{
		FREEBUFFER(new_url,HYIE_MEM_IDX);
  		protocol_free_url(url_parts);
		return -1;
	}

	FREEBUFFER(new_url,HYIE_MEM_IDX);
  	protocol_free_url(url_parts);
	
	return 0;
}


/**
 * Opens an HTTP stream from a web server. 
 *
 * @param url The name of the URL to open.
 * @param referer The URL we were at when entering this new URL.
 * @param headers A pointer to a struct meant to contain information gathered
 * @param headers from the HTTP headers in the response from the server. If
 * @param headers this is set to NULL, do not even try to store any headers.
 *
 * @return the file descriptor for the http stream or a negative value
 * @return if an error occurred.
 */
int protocol_http_open(struct protocol_url *url, 
		       struct protocol_http_headers *headers,
			   struct protocol_http_request *request_headers)
{
  int fd, using_proxy = 0;
  struct protocol_url *proxy_url = NULL;
  char *value;

  //Check if we should use a proxy for this request. 
  protocol_setting_get("http_proxy", &value);
  if(value != NULL) {
	proxy_url = protocol_split_url((char *)value);
	using_proxy = 1;
  } 
  else 
  {
	proxy_url = NULL;
	using_proxy = 0;
  }

//  hyUsbPrintf("Prepare http open\r\n");
  /* Connect and make the request, perhaps through a proxy, perhaps not. */  
  if(using_proxy)
    fd = protocol_http_open_connection(proxy_url);
  else
    fd = protocol_http_open_connection(url);

  if(fd < 0) {
    protocol_free_url(proxy_url);
    return fd;
  }
//  hyUsbPrintf("Prepare to send request\r\n");
  if (protocol_http_make_request(fd, url, using_proxy, headers, request_headers) < 0)
  {
  	  //hyUsbPrintf("\r\nbug is here");
  	  protocol_http_close(fd);
      return -1;
  }

  /* Deal with the return code as we see fit. 
   * Fill in more return codes when we find out what they really stand for.
   */
  return protocol_http_deal_response(fd, using_proxy, url, headers, request_headers);
}


/**
 * Close an HTTP stream.
 *
 * @param fd The file descriptor associated with the stream to close.
 */
int protocol_http_close(int fd)
{
	int ret = -1;
	struct protocol_stream *streamp;
	
	if(fd != -1)
	{
		hyUsbPrintf("close socket =------------ \r\n");
		//先关闭该socket关联的headers
		streamp = protocol_get_stream(fd);
		if(streamp != NULL)
		{
			protocol_free_headers(streamp->headers);
			streamp->headers = NULL;
		}
		ret = closesocket(fd);
		hyUsbPrintf("close socket = %d\r\n",ret);
	}

	return ret;
}




#endif
