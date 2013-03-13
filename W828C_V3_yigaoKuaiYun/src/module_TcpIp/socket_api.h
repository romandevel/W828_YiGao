#ifndef SOCKET_API_H
#define SOCKET_API_H


/*** data operation ***/
/*U16 htons(U16 n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

U16 ntohs(U16 n)
{
	return htons(n);
}

U32 htonl(U32 n)
{
	return ((n & 0xff) << 24) | ((n & 0xff00) << 8) | ((n & 0xff0000) >> 8) | ((n & 0xff000000) >> 24);
}

U32 ntohl(U32 n)
{
	return htonl(n);
}*/

#define htons(n) ( ( ((n)&0xff) << 8 ) | ( ((n)&0xff00) >> 8 ) )

#define ntohs(n) htons(n)

#define htonl(n) ( ( ((n)&0xff) << 24 ) | ( ((n)&0xff00) << 8 ) | ( ((n)&0xff0000) >> 8 ) | ( ((n)&0xff000000) >> 24 ) )

#define ntohl(n) htonl(n)


/*** socket address ***/
#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define socklen_t int

struct in_addr
{
	unsigned long s_addr;
};

struct sockaddr_in
{
	unsigned char sin_len;
	unsigned char sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char	sin_zero[8];
};

struct sockaddr
{
	unsigned char sa_len;
	unsigned char sa_family;
	char	sa_data[14];
};


/*** socket option ***/
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_LDTP		10
//#define SOCK_MTP		11

//Option flags per-socket.
#define SO_DEBUG  0x0001    /* turn on debugging info recording */
#define SO_ACCEPTCONN  0x0002    /* socket has had listen() */
#define SO_REUSEADDR  0x0004    /* allow local address reuse */
#define SO_KEEPALIVE  0x0008    /* keep connections alive */
#define SO_DONTROUTE  0x0010    /* just use interface addresses */
#define SO_BROADCAST  0x0020    /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040    /* bypass hardware when possible */
#define SO_LINGER  0x0080    /* linger on close if data present */
#define SO_OOBINLINE  0x0100    /* leave received OOB data in line */
#define SO_REUSEPORT	0x0200		/* allow local address & port reuse */

#define SO_DONTLINGER   (int)(~SO_LINGER)

//Additional options, not kept in so_options.
#define SO_SNDBUF  0x1001    /* send buffer size */
#define SO_RCVBUF  0x1002    /* receive buffer size */
#define SO_SNDLOWAT  0x1003    /* send low-water mark */
#define SO_RCVLOWAT  0x1004    /* receive low-water mark */
#define SO_SNDTIMEO  0x1005    /* send timeout */
#define SO_RCVTIMEO  0x1006    /* receive timeout */
#define SO_ERROR  0x1007    /* get error status and clear */
#define SO_TYPE    0x1008    /* get socket type */

//Level number for (get/set)sockopt() to apply to socket itself.
#define  SOL_SOCKET  0xffff    /* options for socket level */

//WinSock 2 extension -- manifest constants for shutdown()
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

//Structure used for manipulating linger option.
struct linger
{
	int l_onoff;                /* option on/off */
	int l_linger;               /* linger time */
};


/*** ip option ***/
#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define INADDR_ANY      0
#define INADDR_BROADCAST 0xffffffff
#define INADDR_NONE         ((unsigned long)0xffffffff)  /* 255.255.255.255 */
#define INADDR_LOOPBACK     ((unsigned long)0x7f000001)  /* 127.0.0.1 */

//Options for level IPPROTO_IP
#define IP_TOS       1
#define IP_TTL       2

#define IPTOS_TOS_MASK          0x1E
#define IPTOS_TOS(tos)          ((tos) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY          0x10
#define IPTOS_THROUGHPUT        0x08
#define IPTOS_RELIABILITY       0x04
#define IPTOS_LOWCOST           0x02
#define IPTOS_MINCOST           IPTOS_LOWCOST

//Definitions for IP precedence (also in ip_tos) (hopefully unused)
#define IPTOS_PREC_MASK                 0xe0
#define IPTOS_PREC(tos)                ((tos) & IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL           0xe0
#define IPTOS_PREC_INTERNETCONTROL      0xc0
#define IPTOS_PREC_CRITIC_ECP           0xa0
#define IPTOS_PREC_FLASHOVERRIDE        0x80
#define IPTOS_PREC_FLASH                0x60
#define IPTOS_PREC_IMMEDIATE            0x40
#define IPTOS_PREC_PRIORITY             0x20
#define IPTOS_PREC_ROUTINE              0x00

//Definitions of the bits in an Internet address integer.
//On subnets, host and network parts are found according to
//the subnet mask, not these masks.  */
#define IN_CLASSA(a)        ((((unsigned long)(a)) & 0x80000000) == 0)
#define IN_CLASSA_NET       0xff000000
#define IN_CLASSA_NSHIFT    24
#define IN_CLASSA_HOST      (0xffffffff & ~IN_CLASSA_NET)
#define IN_CLASSA_MAX       128

#define IN_CLASSB(a)        ((((unsigned long)(a)) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET       0xffff0000
#define IN_CLASSB_NSHIFT    16
#define IN_CLASSB_HOST      (0xffffffff & ~IN_CLASSB_NET)
#define IN_CLASSB_MAX       65536

#define IN_CLASSC(a)        ((((unsigned long)(a)) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET       0xffffff00
#define IN_CLASSC_NSHIFT    8
#define IN_CLASSC_HOST      (0xffffffff & ~IN_CLASSC_NET)

#define IN_CLASSD(a)        (((unsigned long)(a) & 0xf0000000) == 0xe0000000)
#define IN_CLASSD_NET       0xf0000000          /* These ones aren't really */
#define IN_CLASSD_NSHIFT    28                  /*   net and host fields, but */
#define IN_CLASSD_HOST      0x0fffffff          /*   routing needn't know. */
#define IN_MULTICAST(a)     IN_CLASSD(a)

#define IN_EXPERIMENTAL(a)  (((unsigned long)(a) & 0xf0000000) == 0xf0000000)
#define IN_BADCLASS(a)      (((unsigned long)(a) & 0xf0000000) == 0xf0000000)

#define IN_LOOPBACKNET      127                 /* official! */

#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = htonl(((unsigned long)((a) & 0xff) << 24) | \
                               ((unsigned long)((b) & 0xff) << 16) | \
                               ((unsigned long)((c) & 0xff) << 8) | \
                                (unsigned long)((d) & 0xff))

#define ip_addr_set(dest, src) (dest)->addr = \
                               ((src) == NULL? 0:\
                               (src)->addr)
//Determine if two address are on the same network.
//arg addr1 IP address 1
//arg addr2 IP address 2
//arg mask network identifier mask
//return !0 if the network identifiers of both address match
#define ip_addr_netcmp(addr1, addr2, mask) (((addr1)->addr & \
                                              (mask)->addr) == \
                                             ((addr2)->addr & \
                                              (mask)->addr))
#define ip_addr_cmp(addr1, addr2) ((addr1)->addr == (addr2)->addr)

#define ip_addr_isany(addr1) ((addr1) == NULL || (addr1)->addr == 0)

//unsigned char ip_addr_isbroadcast(struct ip_addr *addr, struct netif *netif);

#define ip_addr_ismulticast(addr1) (((addr1)->addr & ntohl(0xf0000000)) == ntohl(0xe0000000))

//These are cast to u16_t, with the intent that they are often arguments
//to printf using the U16_F format from cc.h.
#define ip4_addr1(ipaddr) ((u16_t)(ntohl((ipaddr)->addr) >> 24) & 0xff)
#define ip4_addr2(ipaddr) ((u16_t)(ntohl((ipaddr)->addr) >> 16) & 0xff)
#define ip4_addr3(ipaddr) ((u16_t)(ntohl((ipaddr)->addr) >> 8) & 0xff)
#define ip4_addr4(ipaddr) ((u16_t)(ntohl((ipaddr)->addr)) & 0xff)


/*** socket io control ***/
#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    0x7f            /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT         0x40000000      /* copy out parameters */
#define IOC_IN          0x80000000      /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)/* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif

#ifndef FIONREAD
#define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
#endif
#ifndef FIONBIO
#define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
#endif

//Socket I/O Controls
#ifndef SIOCSHIWAT
#define SIOCSHIWAT  _IOW('s',  0, unsigned long)  /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, unsigned long)  /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, unsigned long)  /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, unsigned long)  /* get low watermark */
#define SIOCATMARK  _IOR('s',  7, unsigned long)  /* at oob mark? */
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK    04000U
#endif


/*** select cotrol ***/
#ifndef FD_SET
#define FD_SETSIZE    16
#define FD_SET(n,p)  hyFdSet(n,p)
#define FD_CLR(n,p)  hyFdClr(n,p)
#define FD_ISSET(n,p) hyFdIsSet(n,p)
#define FD_ZERO(p)    ((p)->Count = 0)

typedef struct fd_set
{
	unsigned char Count;
	unsigned char Flag[(FD_SETSIZE+7)/8];
	void *Sock[FD_SETSIZE];
}fd_set;

struct timeval 
{
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};
#endif


/*** DNS ***/
struct hostent
{
    char  *h_name;      /* Official name of the host. */
    char **h_aliases;   /* A pointer to an array of pointers to alternative host names,
                           terminated by a null pointer. */
    int    h_addrtype;  /* Address type. */
    int    h_length;    /* The length, in bytes, of the address. */
    char **h_addr_list; /* A pointer to an array of pointers to network addresses (in
                           network byte order) for the host, terminated by a null pointer. */
#define h_addr h_addr_list[0] /* for backward compatibility */
	volatile int flag;
};


/*** LDTP ***/
#define LDTP_MAX_HOST_NAME_LEN 32

typedef struct ldtp_host
{
	unsigned short Port;
	unsigned long Ip;
	
	unsigned char Name[LDTP_MAX_HOST_NAME_LEN];
	
	struct ldtp_host *pNext;
}tLDTP_HOST;


/*** error code ***/
#define  EPERM     1  /* Operation not permitted */
#define  ENOENT     2  /* No such file or directory */
#define  ESRCH     3  /* No such process */
#define  EINTR     4  /* Interrupted system call */
#define  EIO     5  /* I/O error */
#define  ENXIO     6  /* No such device or address */
#define  E2BIG     7  /* Arg list too long */
#define  ENOEXEC     8  /* Exec format error */
#define  EBADF     9  /* Bad file number */
#define  ECHILD    10  /* No child processes */
#define  EAGAIN    11  /* Try again */
#define  ENOMEM    12  /* Out of memory */
#define  EACCES    13  /* Permission denied */
#define  EFAULT    14  /* Bad address */
#define  ENOTBLK    15  /* Block device required */
#define  EBUSY    16  /* Device or resource busy */
#define  EEXIST    17  /* File exists */
#define  EXDEV    18  /* Cross-device link */
#define  ENODEV    19  /* No such device */
#define  ENOTDIR    20  /* Not a directory */
#define  EISDIR    21  /* Is a directory */
#define  EINVAL    22  /* Invalid argument */
#define  ENFILE    23  /* File table overflow */
#define  EMFILE    24  /* Too many open files */
#define  ENOTTY    25  /* Not a typewriter */
#define  ETXTBSY    26  /* Text file busy */
#define  EFBIG    27  /* File too large */
#define  ENOSPC    28  /* No space left on device */
#define  ESPIPE    29  /* Illegal seek */
#define  EROFS    30  /* Read-only file system */
#define  EMLINK    31  /* Too many links */
#define  EPIPE    32  /* Broken pipe */
#define  EDOM    33  /* Math argument out of domain of func */
#define  ERANGE    34  /* Math result not representable */
#define  EDEADLK    35  /* Resource deadlock would occur */
#define  ENAMETOOLONG  36  /* File name too long */
#define  ENOLCK    37  /* No record locks available */
#define  ENOSYS    38  /* Function not implemented */
#define  ENOTEMPTY  39  /* Directory not empty */
#define  ELOOP    40  /* Too many symbolic links encountered */
#define  EWOULDBLOCK  EAGAIN  /* Operation would block */
#define  ENOMSG    42  /* No message of desired type */
#define  EIDRM    43  /* Identifier removed */
#define  ECHRNG    44  /* Channel number out of range */
#define  EL2NSYNC  45  /* Level 2 not synchronized */
#define  EL3HLT    46  /* Level 3 halted */
#define  EL3RST    47  /* Level 3 reset */
#define  ELNRNG    48  /* Link number out of range */
#define  EUNATCH    49  /* Protocol driver not attached */
#define  ENOCSI    50  /* No CSI structure available */
#define  EL2HLT    51  /* Level 2 halted */
#define  EBADE    52  /* Invalid exchange */
#define  EBADR    53  /* Invalid request descriptor */
#define  EXFULL    54  /* Exchange full */
#define  ENOANO    55  /* No anode */
#define  EBADRQC    56  /* Invalid request code */
#define  EBADSLT    57  /* Invalid slot */

#define  EDEADLOCK  EDEADLK

#define  EBFONT    59  /* Bad font file format */
#define  ENOSTR    60  /* Device not a stream */
#define  ENODATA    61  /* No data available */
#define  ETIME    62  /* Timer expired */
#define  ENOSR    63  /* Out of streams resources */
#define  ENONET    64  /* Machine is not on the network */
#define  ENOPKG    65  /* Package not installed */
#define  EREMOTE    66  /* Object is remote */
#define  ENOLINK    67  /* Link has been severed */
#define  EADV    68  /* Advertise error */
#define  ESRMNT    69  /* Srmount error */
#define  ECOMM    70  /* Communication error on send */
#define  EPROTO    71  /* Protocol error */
#define  EMULTIHOP  72  /* Multihop attempted */
#define  EDOTDOT    73  /* RFS specific error */
#define  EBADMSG    74  /* Not a data message */
#define  EOVERFLOW  75  /* Value too large for defined data type */
#define  ENOTUNIQ  76  /* Name not unique on network */
#define  EBADFD    77  /* File descriptor in bad state */
#define  EREMCHG    78  /* Remote address changed */
#define  ELIBACC    79  /* Can not access a needed shared library */
#define  ELIBBAD    80  /* Accessing a corrupted shared library */
#define  ELIBSCN    81  /* .lib section in a.out corrupted */
#define  ELIBMAX    82  /* Attempting to link in too many shared libraries */
#define  ELIBEXEC  83  /* Cannot exec a shared library directly */
#define  EILSEQ    84  /* Illegal byte sequence */
#define  ERESTART  85  /* Interrupted system call should be restarted */
#define  ESTRPIPE  86  /* Streams pipe error */
#define  EUSERS    87  /* Too many users */
#define  ENOTSOCK  88  /* Socket operation on non-socket */
#define  EDESTADDRREQ  89  /* Destination address required */
#define  EMSGSIZE  90  /* Message too long */
#define  EPROTOTYPE  91  /* Protocol wrong type for socket */
#define  ENOPROTOOPT  92  /* Protocol not available */
#define  EPROTONOSUPPORT  93  /* Protocol not supported */
#define  ESOCKTNOSUPPORT  94  /* Socket type not supported */
#define  EOPNOTSUPP  95  /* Operation not supported on transport endpoint */
#define  EPFNOSUPPORT  96  /* Protocol family not supported */
#define  EAFNOSUPPORT  97  /* Address family not supported by protocol */
#define  EADDRINUSE  98  /* Address already in use */
#define  EADDRNOTAVAIL  99  /* Cannot assign requested address */
#define  ENETDOWN  100  /* Network is down */
#define  ENETUNREACH  101  /* Network is unreachable */
#define  ENETRESET  102  /* Network dropped connection because of reset */
#define  ECONNABORTED  103  /* Software caused connection abort */
#define  ECONNRESET  104  /* Connection reset by peer */
#define  ENOBUFS    105  /* No buffer space available */
#define  EISCONN    106  /* Transport endpoint is already connected */
#define  ENOTCONN  107  /* Transport endpoint is not connected */
#define  ESHUTDOWN  108  /* Cannot send after transport endpoint shutdown */
#define  ETOOMANYREFS  109  /* Too many references: cannot splice */
#define  ETIMEDOUT  110  /* Connection timed out */
#define  ECONNREFUSED  111  /* Connection refused */
#define  EHOSTDOWN  112  /* Host is down */
#define  EHOSTUNREACH  113  /* No route to host */
#define  EALREADY  114  /* Operation already in progress */
#define  EINPROGRESS  115  /* Operation now in progress */
#define  ESTALE    116  /* Stale NFS file handle */
#define  EUCLEAN    117  /* Structure needs cleaning */
#define  ENOTNAM    118  /* Not a XENIX named type file */
#define  ENAVAIL    119  /* No XENIX semaphores available */
#define  EISNAM    120  /* Is a named type file */
#define  EREMOTEIO  121  /* Remote I/O error */
#define  EDQUOT    122  /* Quota exceeded */

#define  ENOMEDIUM  123  /* No medium found */
#define  EMEDIUMTYPE  124  /* Wrong medium type */


#define ENSROK    0 /* DNS server returned answer with no data */
#define ENSRNODATA  160 /* DNS server returned answer with no data */
#define ENSRFORMERR 161 /* DNS server claims query was misformatted */
#define ENSRSERVFAIL 162  /* DNS server returned general failure */
#define ENSRNOTFOUND 163  /* Domain name not found */
#define ENSRNOTIMP  164 /* DNS server does not implement requested operation */
#define ENSRREFUSED 165 /* DNS server refused query */
#define ENSRBADQUERY 166  /* Misformatted DNS query */
#define ENSRBADNAME 167 /* Misformatted domain name */
#define ENSRBADFAMILY 168 /* Unsupported address family */
#define ENSRBADRESP 169 /* Misformatted DNS reply */
#define ENSRCONNREFUSED 170 /* Could not contact DNS servers */
#define ENSRTIMEOUT 171 /* Timeout while contacting DNS servers */
#define ENSROF    172 /* End of file */
#define ENSRFILE  173 /* Error reading file */
#define ENSRNOMEM 174 /* Out of memory */
#define ENSRDESTRUCTION 175 /* Application terminated lookup */
#define ENSRQUERYDOMAINTOOLONG  176 /* Domain name is too long */
#define ENSRCNAMELOOP 177 /* Domain name is too long */

#ifndef errno
extern int errno;
#endif


/*** socket interface ***/
#define socket(domain,type,protocol)	hySocket(domain,type,protocol)
#define bind(s,name,namelen)			hyBind(s,name,namelen)
#define listen(s,backlog) 				hyListen(s,backlog)
#define accept(s,addr,addrlen)			hyAccept(s,addr,addrlen)
#define connect(s,name,namelen)			hyConnect(s,name,namelen)
#define shutdown(s,how)					hyShutdown(s,how)
#define closesocket(s) 					hyClose(s)
#define select(maxfdp1,readset,writeset,exceptset,timeout) hySelect(maxfdp1,readset,writeset,exceptset,timeout)
#define recv(s,mem,len,flags)			hyRecv(s,mem,len,flags)
#define recvfrom(s,mem,len,flags,from,fromlen)	hyRecvfrom(s,mem,len,flags,from,fromlen)
#define send(s,dataptr,size,flags)		hySend(s,dataptr,size,flags)
#define sendto(s,dataptr,size,flags,to,tolen)	hySendto(s,dataptr,size,flags,to,tolen)
//#define read(s,mem,len)					hyRead(s,mem,len)
//#define write(s,dataptr,size)			hyWrite(s,dataptr,size)
#define getpeername(s,name,namelen)		hyGetpeername(s,name,namelen)
#define getsockname(s,name,namelen)		hyGetsockname(s,name,namelen)
#define getsockopt(s,level,optname,optval,optlen)	hyGetsockopt(s,level,optname,optval,optlen)
#define setsockopt(s,level,optname,optval,optlen)	hySetsockopt(s,level,optname,optval,optlen)
#define ioctlsocket(s,cmd,argp)			hyIoctl(s,cmd,argp)
#define gethostbyname(name)				hyGetHostByName(name)
#define gethostbyname_noblock(name)		hyGetHostByName_NoBlock(name)
#define checkhostdata(host)				hyCheckHostData(host)
#define getlocalip()					hyGetLocalIp()

#define ldtpopen(s,name)				hyLdtpOpen(s,name)
#define ldtpscan(s)						hyLdtpScan(s)
#define ldtpgethost(s,num,host)			hyLdtpGetHost(s,num,host)
#define ldtprecv(s,host,data,size)		hyLdtpRecv(s,host,data,size)
#define ldtpsend(s,host,data,size)		hyLdtpSend(s,host,data,size)


int hySocket(int domain, int type, int protocol);
int hyBind(int s, struct sockaddr *name, int namelen);
int hyListen(int s, int backlog);
int hyAccept(int s, struct sockaddr *addr, int *addrlen);
int hyConnect(int s, struct sockaddr *name, int namelen);
int hyShutdown(int s, int how);
int hyClose(int s);
int hySelect(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,struct timeval *timeout);
int hyRecv(int s, void *mem, int len, unsigned int flags);
int hyRecvfrom(int s, void *mem, int len, unsigned int flags,struct sockaddr *from, int *fromlen);
int hySend(int s, void *dataptr, int size, unsigned int flags);
int hySendto(int s, void *dataptr, int size, unsigned int flags,struct sockaddr *to, int tolen);
int hyRead(int s, void *mem, int len);
int hyWrite(int s, void *dataptr, int size);
int hyGetpeername (int s, struct sockaddr *name, int *namelen);
int hyGetsockname (int s, struct sockaddr *name, int *namelen);
int hyGetsockopt (int s, int level, int optname, void *optval, int *optlen);
int hySetsockopt (int s, int level, int optname, const void *optval, int optlen);
int hyIoctl(int s, long cmd, void *argp);
struct hostent *hyGetHostByName(const char *name);
struct hostent *hyGetHostByName_NoBlock(const char *name);
int hyCheckHostData(struct hostent *host);
char *hyGetLocalIp(void);

int hyLdtpOpen(int s,unsigned char *name);
int hyLdtpScan(int s);
int hyLdtpGetHost(int s,int *num,struct ldtp_host **host);
int hyLdtpRecv(int s,struct ldtp_host *host,void *data,int size);
int hyLdtpSend(int s,struct ldtp_host *host,void *data,int size);


/*** other ***/
unsigned long inet_addr(const char *cp);
int inet_aton(const char *cp, struct in_addr *addr);
char *inet_ntoa(struct in_addr addr);


#endif