#include "comm.h"
#include "comm_image_api.h"

#define WRITE_REG32(pAddr,Value) (*((volatile U32 *)(pAddr))=(U32)(Value))
#define READ_REG32(pAddr) (*((volatile U32 *)(pAddr)))
#define SET_REG(pAddr,Mask,Value) ( *((volatile U32 *)(pAddr)) = (*((volatile U32 *)(pAddr)) & ~(Mask)) | (Value) )

#define HWADDR(Addr) ( (U32)hsaConvertUnCacheMemory((void *)(Addr)) )