#ifndef _HTTPS_H_
#define _HTTPS_H_

//#include "win.h"
#ifdef HAVE_SSL
#include "crypto.h"
#include "ssl.h"
//#include "err.h"
#include "rand.h"

extern int  protocol_https_init(void);
extern int  protocol_https_destroy(void);
extern SSL *protocol_https_connect(int sock);
extern int  protocol_https_close(int sock, SSL *ssl);

#endif  //HAVE_SSL
#endif