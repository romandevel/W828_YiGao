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
// FILE NAME:    cmd_glb.h
//
// VERSION:      <1.00.00.0001>
//
// DESCRIPTION:  This header file declares the definition of commands
//               THis file is a GLOBAL shared header file.
//               Here doesn't have any exported/internal function list.
//
// DOCUMENT REF: SZLAB/PRS0003
//
// NOTES:        Please keep it updated!
//-----------------------------------------------------------------------------
//

#ifndef _COMMAND_GLOBAL_H_ //-----------------
#define _COMMAND_GLOBAL_H_

//-----------------------------------------------------------------------------
// Standard include files:
//-----------------------------------------------------------------------------
//

//-----------------------------------------------------------------------------
// Project include files:
//-----------------------------------------------------------------------------
//

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------
// Types and defines:
//-----------------------------------------------------------------------------
//

// global command declaration
// All of modules must support these command listed below.
//   >> command declaration format:
//   >> line 1 : #define cCMD_xxxx   n
//   >> line 2 : // description of command
// Every command can be followed by at most 3 parameters.
// The three parameters are separately called kData, kExtData1, kExtData2.
// The description must follow the command definition line at once, it must 
// contain the contents mentioned below:
//   part 1 : the description about command, e.g. purpose, functionality.
//   part 2 : the description about parameters, e.g. data type, meaning

#define cCMD_START              0
// ** To activate the module, and then the module will handle the requests
// If the module is already active, simply ignore this command
// If the module is successful activated,  return cSSA_OK, otherwise return 
// other error code.
// ** This command has 1 running parameter.
// Parameter 1 : kData, a boolean flag to determine whether the main function 
//               of module should be started, is a Bool type parameter.

#define cCMD_STOP               1
// ** To make the module stop handling request & control
// If the module is already idle, simply ignore this command
// If the module is successful stopped,  return cSSA_OK, otherwise return 
// other error code.
// ** This command doesn't have running parameter.

#define cCMD_SET_RUNNING_MODE   2
// ** To set the module to normal or background running mode
// If the module is already in the wanted mode, simply ignore this command
// If the running mode is successful changed,  return cSSA_OK, otherwise return 
// other error code.
// ** This command has 1 parameter.
// Parameter 1 : kData, which type is the RUNNING_MODE_e.

#define cCMD_DISPATCH_MESSAGE   3
// ** To send a message to the module
// If the message is processed, return cSSA_OK
// If the message is not recognized and the message type is a keyb message, 
// return cSSA_KEYB_MSG_UNKNOWN.
// If the message is not recognized and the message type is not a keyb 
// message, return cSSA_MSG_UNKNOWN_TYPE.
// ** This command have variant parameters with different message type(kData).
// Parameter 1 : kData, message type
// Parameter 2 : kExtData1, the additional information
// Parameter 3 : kExtData2, the additional information
// the combination of message type and additional data is illustrated below:
//   message type(kData)    kExtData1                       kExtData2
//   cMSG_TIMER             U32 timeInMillisecond           N/A
//   cMSG_PSA_EVENT         PSA_ActiveFeedback_en psaEvent  N/A
//   cMSG_KEYB              Keyb_Code_e keybCode            U32 keybEventType


// module command base declaration
// the name of command base should be in format "cCMD_BASE_<MODULE>"
// the minimum command base is 0x100, every command base must be integer 
// times of 0x100.
//#define cCMD_BASE_
#define cCMD_BASE_MP3		0x100
#define cCMD_BASE_RADIO	0x200
#define cCMD_BASE_RECORD	0x300

// message type declaration
#define cMSG_TIMER              0
#define cMSG_HSA_EVENT          1
#define cMSG_KEYB               2
#define cMSG_ACTION		3


#ifdef __cplusplus
}
#endif

#endif // COMMAND_GLOBAL_H //---------------

/*============================ History List ================================
 2. Rev 1.00.00.0001, 2003/05/12 18:01             Xuefeng Yang
    add the declaration of MP3, Radio, Record module command base
    add one parameter for the global command "cCMD_START"
 1. Rev 1.00.00.0000, 2003/05/09 18:05             Xuefeng Yang
============================================================================*/