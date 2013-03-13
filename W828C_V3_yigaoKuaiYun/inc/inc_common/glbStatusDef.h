//-----------------------------------------------------------------------------
// (C) Copyright 2003 Philips Semiconductors, All rights reserved
//
// This source code and any compilation or derivative thereof is the sole
// property of Philips Corporation and is provided pursuant to a Software
// License Agreement.  This code is the proprietary information of Philips
// Corporation and is confidential in nature.  Its use and dissemination by
// any party other than Philips Corporation is strictly limited by the
// confidential information provisions of the Agreement referenced above.
//-----------------------------------------------------------------------------
// FILE NAME:    global.h
//
// VERSION:      1.00.00.0005
//
// DESCRIPTION:  This header file includes the global shared declarations.
//               THis file is a GLOBAL shared header file.
//               Here doesn't have any exported/internal function list.
//
// DOCUMENT REF: SZLAB/PRS0003
//
// NOTES:        Please keep it updated!
//-----------------------------------------------------------------------------
//

#ifndef _GLOBAL_H_ //-----------------
#define _GLOBAL_H_


#ifdef __cplusplus
extern "C"
{
#endif

// conversion between Second/Minute and ticks
#define TICK_PER_SEC        (1000/50) //jie (1000/50)       // 50ms per tick
#define TICK_PER_MIN        (TICK_PER_SEC * 60)
#define SEC2TICK(x)         (x * TICK_PER_SEC)
#define MIN2TICK(x)         (x * TICK_PER_MIN)
#define TICK2SEC(x)         ((x + TICK_PER_SEC/2) / TICK_PER_SEC)
#define TICK2MIN(x)         ((x + TICK_PER_MIN/2) / TICK_PER_MIN)

// the return status code used in the whole project
#define cSSA_OK                     0x0
#define cSSA_NOT_OK                 0x1   // genenral error
#define cSSA_BAD_PARAMETER          0x2   // wrong parameter(s)
#define cSSA_NOT_INITIALIZED        0x3   // initialization is not completed yet
#define cSSA_OUT_OF_RESOURCE        0x4   // not enough resource to complete the operation
#define cSSA_HW_FAILED              0x5   // hardware is failed
#define cSSA_BUSY                   0x6   // component is busy
#define cSSA_UNKNOWN_COMMAND        0x7   // not supported command
#define cSSA_COMMAND_REJECTED       0x8   // command is rejected as it can't be completed under the current component state
#define cSSA_MSG_UNKNOWN_TYPE       0x9   // not supported message type
#define cSSA_KEYB_MSG_UNKNOWN       0xA  // not supported/processed keyb message
#define cSSA_HSA_MSG_UNKNOWN        0xB  // not supported/processed PSA message
#define cSSA_ENTER_DIALOG_STATUS    0xC  // enter modeless dialog status
#define cSSA_LEAVE_DIALOG_STATUS    0xD  // leave modeless dialog status

#define cSSA_TASK_FINISH            0xE  // task finishes and should be closed
#define cSSA_TASK_TO_BACKGROUND  0x8E

#define cSSA_TASK_SWITHCH_TO_TUNER_AST		0x0F
#define cSSA_TASK_SWITHCH_TO_TUNER_SEARCH		0x8F
#define cSSA_TASK_SWITHCH_TO_PLAYBACK		0x10

/*
#define cSSA_TASK_SWITCH_TO_PLAYER  0xF  // task finishes and player should be launched
#define cSSA_TASK_SWITCH_TO_LINEREC 0x10  // task finishes and voice record should be launched
#define cSSA_TASK_SWITCH_TO_MICREC  0x11  // task finishes and voice record should be launched
#define cSSA_TASK_SWITCH_TO_FMRADIO 0x12  // task finishes and FM radio should be launched
#define cSSA_TASK_SWITCH_TO_MENU    0x13
#define cSSA_TASK_SWITCH_TO_LEARNER 0x14  // task finishes and Language learning machine should be launched

#define cSSA_BACK_SWITCH_TO_MENU    0x15
#define cSSA_BACK_SWITCH_TO_VOLUME  0x16
#define cSSA_BACK_SWITCH_TO_FMREC  	0x17
#define cSSA_BACK_SWITCH_TO_EQ      0x18
#define cSSA_BACK_SWITCH_TO_BROWSE  0x19

#define cSSA_VOLUME_SWITCH_TO_BROWSE	0x1E
#define cSSA_VOLUME_SWITCH_TO_MENU      0x1F
#define cSSA_VOLUME_SWITCH_TO_OTHER     0x20
#define cSSA_VOLUME_SWITCH_TO_RECORD    0x21
#define cSSA_VOLUME_SWITCH_TO_FMRADIO   0x22

#define cSSA_BROWSE_FINISH_TO_PLAY      	0x23 //BROWSE end himself and BEGIN TO PLAY.
#define cSSA_BROWSE_FINISH_TO_MENU          0x24 //browse close himself and open menu module.untouch backwork module
#define cSSA_BROWSE_FINISH_TO_TUNER_OR_MP3  0x25 // 关闭browse,判断后台模块如果是mp3,则启动mp3,和volume,otherwise ,do nothing.
#define cSSA_MENU_TASK_SWITCH_TO_PLAYER  	0x26  //menu  task finishes and player should be launched
#define cSSA_MENU_TASK_SWITCH_TO_RECORD  	0x27  //menu  task finishes and record should be launched (VOICE OR LINEIN)
#define cSSA_MENU_TASK_SWITCH_TO_FMRADIO 	0x28  // menu task finishes and FM radio should be launched
#define cSSA_MENU_TASK_SWITCH_TO_LEARNER  	0x29  // menu task finishes and Language learning machine should be launched

#define cSSA_TASK_SWITCH_TO_BROWSE              0x30 // task finishes and Language learning machine should be launched
*/

// the module operating mode
typedef enum _RUNNING_MODE_e 
{
    RUNNING_MODE_NORMAL ,    // input/output operations are unrestricted.
    RUNNING_MODE_BACKGROUND,      // limited input operations, ouput operation is forbidden
    RUNNING_MODE_LIMIT
} RUNNING_MODE_e, *pRUNNING_MODE_e;

//-----------------------------------------------------------------------------
// Exported functions:
//-----------------------------------------------------------------------------
//

#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H //---------------

/*============================ History List ================================
 6. Rev 1.00.00.0005, 2003/07/24 18:05             Xuefeng Yang, Rock
    add a time constant and related macros used to convert time from tick to 
    sec/min and reverse this conversion.
    add a series of return code for task switching purpose 
 5. Rev 1.00.00.0004, 2003/05/21 16:05             Xuefeng Yang
    add some error codes "cSSA_BAD_PARAMETER/cSSA_NOT_OK/cSSA_TASK_xxx" and
    "cSSA_PSA_MSG_UNKNOWN".
 4. Rev 1.00.00.0003, 2003/05/13 17:49             Xuefeng Yang
    add an error code "cSSA_COMMAND_REJECTED".
 3. Rev 1.00.00.0002, 2003/05/12 19:42             Xuefeng Yang
    add a missing error code "cSSA_MSG_UNKNOWN_TYPE".
    add an error code "cSSA_OUT_OF_RESOURCE".
 2. Rev 1.00.00.0001, 2003/05/12 10:39             Xuefeng Yang
    add the definition of enum type "_RUNNING_MODE_e".
 1. Rev 1.00.00.0000, 2003/05/09 18:27             Xuefeng Yang
============================================================================*/
