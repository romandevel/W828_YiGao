/*************************************************************
%
% Filename     : hyOsCpuCfg.h
% Project name : Os and Cpu definition
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040106  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%	This source file contains the os and cpu definition  
%	这个是定义使用的os和cpu平台的文件，我们现在使用的是shark芯片
%	hyos，所以这些定义并没有用到
%	需要根据具体情况定义
****************************************************************/

#ifndef _HY_OS_CPU_CFG_
#define _HY_OS_CPU_CFG_


/*	定义使用的cpu和os */ 		
#define HYCFG_BUILD_VERSION            00.00.00
#define HYCFG_CPU                      0x00050009
#define HYCFG_ENDIAN                   0
#define HYCFG_OS                       0x08000000

#define HYCFG_CPU_IS_X86               0
#define HYCFG_CPU_IS_MIPS              0
#define HYCFG_CPU_IS_HP                0
#define HYCFG_CPU_IS_TM                0
#define HYCFG_CPU_IS_ARM               1
#define HYCFG_CPU_IS_REAL              0

#define HYCFG_OS_IS_BTM                0
#define HYCFG_OS_IS_CE                 0
#define HYCFG_OS_IS_NT                 0
#define HYCFG_OS_IS_PSOS               0
#define HYCFG_OS_IS_NULLOS             0
#define HYCFG_OS_IS_ECOS               0
#define HYCFG_OS_IS_VXWORKS            0
#define HYCFG_OS_IS_MTOS               0
#define HYCFG_OS_IS_CEXEC              1



/*	各种os和cpu的定义 */
#define HYCFG_CPU_TYPE_MASK            0xffff0000
#define HYCFG_CPU_TYPE_X86             0x00010000
#define HYCFG_CPU_TYPE_MIPS            0x00020000
#define HYCFG_CPU_TYPE_TM              0x00030000
#define HYCFG_CPU_TYPE_HP              0x00040000
#define HYCFG_CPU_TYPE_ARM             0x00050000
#define HYCFG_CPU_TYPE_REAL            0x00060000

#define HYCFG_CPU_MODEL_MASK           0x0000ffff
#define HYCFG_CPU_MODEL_I486           0x00000001
#define HYCFG_CPU_MODEL_R3940          0x00000002
#define HYCFG_CPU_MODEL_R4300          0x00000003
#define HYCFG_CPU_MODEL_TM1100         0x00000004
#define HYCFG_CPU_MODEL_TM1300         0x00000005
#define HYCFG_CPU_MODEL_TM32           0x00000006
#define HYCFG_CPU_MODEL_HP             0x00000007
#define HYCFG_CPU_MODEL_R4640          0x00000008
#define HYCFG_CPU_MODEL_ARM7           0x00000009
#define HYCFG_CPU_MODEL_ARM920T        0x0000000a
#define HYCFG_CPU_MODEL_ARM940T        0x0000000b
#define HYCFG_CPU_MODEL_ARM10          0x0000000c
#define HYCFG_CPU_MODEL_STRONGARM      0x0000000d
#define HYCFG_CPU_MODEL_RD24120        0x0000000e
#define HYCFG_CPU_MODEL_ARM926EJS      0x0000000f
#define HYCFG_CPU_MODEL_ARM946         0x00000010
#define HYCFG_CPU_MODEL_R1910          0x00000011
#define HYCFG_CPU_MODEL_R4450          0x00000012
#define HYCFG_CPU_MODEL_TM3260         0x00000013

#define HYCFG_ENDIAN_BIG               1
#define HYCFG_ENDIAN_LITTLE            0
#define HYCFG_OS_MASK                  0xff000000
#define HYCFG_OS_VERSION_MASK          0x00ffffff
#define HYCFG_OS_BTM                   0x00000000
#define HYCFG_OS_CE                    0x01000000
#define HYCFG_OS_CE212                 0x01020102
#define HYCFG_OS_CE300                 0x01030000
#define HYCFG_OS_NT                    0x02000000
#define HYCFG_OS_NT4                   0x02040000
#define HYCFG_OS_PSOS                  0x03000000
#define HYCFG_OS_PSOS250               0x03020500
#define HYCFG_OS_PSOS200               0x03020000
#define HYCFG_OS_NULLOS                0x04000000
#define HYCFG_OS_ECOS                  0x05000000
#define HYCFG_OS_VXWORKS               0x06000000
#define HYCFG_OS_MTOS                  0x07000000
#define HYCFG_OS_CEXEC                 0x08000000
#define HYCFG_SCOPE_SP                 0
#define HYCFG_SCOPE_MP                 1
#define HYCFG_REL_ASSERT               0x00000002
#define HYCFG_REL_DEBUG                0x00000001
#define HYCFG_REL_RETAIL               0x00000000
#define HYCFG_CPU_I486                 0x00010001
#define HYCFG_CPU_R3940                0x00020002
#define HYCFG_CPU_R4300                0x00020003
#define HYCFG_CPU_TM1100               0x00030004
#define HYCFG_CPU_TM1300               0x00030005
#define HYCFG_CPU_TM32                 0x00030006
#define HYCFG_CPU_HP                   0x00040007
#define HYCFG_CPU_R4640  	       		0x00020008
#define HYCFG_CPU_ARM7   	       		0x00050009
#define HYCFG_CPU_ARM920T	       		0x0005000a
#define HYCFG_CPU_ARM940T	       		0x0005000b
#define HYCFG_CPU_ARM10	            0x0005000c
#define HYCFG_CPU_STRONGARM            0x0005000d
#define HYCFG_CPU_RD24120              0x0006000e
#define HYCFG_CPU_ARM926EJS            0x0005000f
#define HYCFG_CPU_ARM946               0x00050010
#define HYCFG_CPU_R1910  	       		0x00020011
#define HYCFG_CPU_R4450  	       		0x00020012
#define HYCFG_CPU_TM3260  	       		0x00030013

#define HYCFG_MODE_KERNEL              1
#define HYCFG_MODE_USER                0

 #endif   /* !defined(_HY_OS_CPU_CFG_)                                     	*/
