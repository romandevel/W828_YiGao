#ifndef __GSMD_SELECT_H
#define __GSMD_SELECT_H


#include "llist.h"
#include "gsmd.h"


//#ifdef __GSMD__


#define GSMD_FD_READ		0x0001
#define GSMD_FD_WRITE		0x0002
#define GSMD_FD_WRITE_GPRS	0x0004
#define GSMD_FD_EXCEPT		0x0008




extern int gsmd_register_fd(struct gsmd_fd *ufd);
extern void gsmd_unregister_fd(struct gsmd_fd *ufd);
extern int gsmd_select_main(struct gsmd *g);

//#endif /* __GSMD__ */

#endif
