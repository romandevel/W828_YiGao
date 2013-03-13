/*
*	File: dns.c this currently only supports blocking Dns scheme 
*	banqhu@gmail.com
*/

/*
*	this modle is very important in the programe,
*   later,we will use ---- `Non bloching thread-handled Dns scheme`
*/

#include "IEdns.h"
#include "http.h"

DnsCache *dns_cache;
static int dns_cache_size=0, dns_cache_size_max=0;


/*
*	initializer founction
*/
void a_Dns_init(void)
{
	dns_cache_size = 0;
	dns_cache_size_max = DNS_MAX_CACHE;
	dns_cache = (DnsCache *)GETBUFFER(dns_cache_size_max*sizeof(DnsCache),HYIE_MEM_IDX);	
	dns_cache->addr_list = NULL;
	dns_cache->hostname = NULL;
	
}


/*
*	first,allocate an 'alloc_step' sized chunk,after that,double
*	the list size ---to make it faster(I think~)!
*/
int List_add(DnsCache **cache_list,int num_items,int alloc_step) 
{

	if ( num_items >= alloc_step )
	{ 
	  while ( num_items >= alloc_step )
	  {
		 alloc_step <<= 1;
	  }
	  dns_cache_size_max = alloc_step;
	  *cache_list = (DnsCache *)REGETBUFFER(*cache_list, alloc_step * sizeof(DnsCache),HYIE_MEM_IDX);
	}

	if (*cache_list == NULL)
	{
	   return -1;
	}else
	{
	   return 1;
	}
 }


/*
*  Add an IP/hostname pair to Dns-cache
*/
static void Dns_cache_add(const char *hostname, WebHost *host)
{
	List_add(&dns_cache, dns_cache_size, dns_cache_size_max);
	dns_cache[dns_cache_size].hostname = Strdup(hostname);
	dns_cache[dns_cache_size].addr_list = host;
	++dns_cache_size;
}


/*
 * Allocate a host structure and add it to the list
 */
WebHost *Dns_note_hosts(const char *hostname)
{
	struct hostent *hent;
	WebHost *wh;
	int state;

	hent = gethostbyname_noblock(hostname);
	while (1)
	{
		state = checkhostdata(hent);  //>0正确 =0错误或超时 <0正在处理
		if (state > 0)
			break;
		sleep(2);
		if (Net_GsmReset() == 1 || state < 0)
			return NULL;
	}

	wh = (WebHost *)GETBUFFER(sizeof(WebHost),HYIE_MEM_IDX);
	if (wh == NULL)
		return NULL;
	wh->alen = hent->h_length;
	wh->data = (char *)GETBUFFER(hent->h_length,HYIE_MEM_IDX);
	if (wh->data == NULL)
	{
		FREEBUFFER(wh,HYIE_MEM_IDX);
		return NULL;
	}
	
	memcpy(wh->data, hent->h_addr_list[0], hent->h_length);

	// DNS succeeded, let's cache it 
	Dns_cache_add(hostname, wh);	
	return wh;
}


/*
*	return the IP for given hostname 
*/
WebHost *Dns_server_req(const char *hostname)
{
	int i=0;
	WebHost *wh;
	
	//check for cache hit. 
	for (i = 0; i < dns_cache_size; i++)
	{
		if (!_stricmp(hostname, dns_cache[i].hostname))
		 break;
	}

	// if it hits already resolved, return inmediately
	if (i < dns_cache_size) 
	{
		return dns_cache[i].addr_list;
	}else
	{
		wh = Dns_note_hosts(hostname);
		return wh;
	}
}


/*
*	Dns memory deallocation
*	call this at exit time
*/
void Dns_cache_free()
{
	int i;
	if (dns_cache)
	{
		for ( i = 0; i < dns_cache_size; ++i )
		{
			if (dns_cache[i].addr_list)
			{
				if (dns_cache[i].addr_list->data)
				{
					FREEBUFFER(dns_cache[i].addr_list->data,HYIE_MEM_IDX);
				}
				FREEBUFFER(dns_cache[i].addr_list,HYIE_MEM_IDX);
			}
			
			if (dns_cache[i].hostname)
			{
				FREEBUFFER(dns_cache[i].hostname,HYIE_MEM_IDX);
			}		
		}
		dns_cache_size = 0;
		FREEBUFFER(dns_cache,HYIE_MEM_IDX);	
	}
}