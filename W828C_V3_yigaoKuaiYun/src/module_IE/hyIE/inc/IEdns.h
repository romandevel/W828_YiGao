/** 
	File: dns.c
 */

#ifndef _PROTOCOL_DNS_H_
#define _PROTOCOL_DNS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "socket_api.h"	

#define DNS_MAX_CACHE 10

typedef struct _WebHost
{
	int alen;
	char *data;
} WebHost;

typedef struct _WebHostBak
{
	int alen;
	char data[10];
	
} WebHostBak;

typedef struct DnsCacheList 
{
	char *hostname;          /* host name for cache */
	WebHost *addr_list ;
} DnsCache;


/* Function prototypes. */
//extern WebHost *Dns_server_req(const char *hostname);
//extern void a_Dns_init(void);
//extern void Dns_cache_free();

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_HTTP_H_ */
