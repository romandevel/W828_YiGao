#ifndef CSR_ERROR_H__
#define CSR_ERROR_H__
/****************************************************************************

               (c) Cambridge Silicon Radio Limited 2009

               All rights reserved and confidential information of CSR

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* function return error codes using C/POSIX standards. */
#define ENOENT			2	/* no such file or directory */ 
#define EINTR			4	/* interrupted system call */ 
#define EIO				5	/* i/o error */ 
#define ENOMEM			12	/* out of memory */ 
#define EFAULT			14	/* bad address */ 
#define ENODEV			19	/* no such device */ 
#define EINVAL			22	/* invalid argument */ 
#define ENOSPC			28	/* no space left on device */ 
#define ERANGE			34	/* argument out of range */
#define ENODATA			61
#define ETIMEDOUT		110

#define CSR_ENOENT		ENOENT
#define CSR_EIO			EIO
#define CSR_ENOMEM		ENOMEM
#define CSR_EFAULT		EFAULT
#define CSR_EEXIST		EIO
#define CSR_ENODEV		ENODEV
#define CSR_EINVAL		EINVAL
#define CSR_ENOSPC		ENOSPC
#define CSR_ERANGE		ERANGE
#define CSR_ENODATA		ENODATA
#define CSR_ETIMEDOUT	ETIMEDOUT
#define CSR_ENOTSUP		EIO

#ifdef __cplusplus
}
#endif

#endif
