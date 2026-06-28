/*
 * can_message.h
 *
 *  Created on: 2026年5月23日
 *      Author: Administrator
 */

#ifndef INTERFACE_CAN_MESSAGE_H_
#define INTERFACE_CAN_MESSAGE_H_

/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/

#include "Platform_Types.h"

#include <stdio.h>

/*****************************************************************************
 *  Defines
 *****************************************************************************/
#define INVALID_CAN_ID 0xFFFFFFFF

/*****************************************************************************
 * Exported Type Declarations (enum, struct, union, typedef)
 *****************************************************************************/
typedef struct
{
    uint32  ID;
    uint8  Length;
    uint8  Extended;
    uint8  fd_msg;
    uint8  Data[64];
    uint8  available_flg;
} CAN_DATATYPE;

typedef struct
{
    uint32  ID;
    uint16  Length;
    uint8  Extended;
    uint8  fd_msg;
    uint8  Data[1800];
    uint8  available_flg;
} CAN_1939DATATYPE;

typedef struct
{
    uint32 wakeup_id;
    uint32 wakeup_id_mask;
    uint8  wakeup_id_format;
    uint8  wakeup_enable;
}CAN_TRCV_RTE_INIF_TYPEDEF;

/*****************************************************************************
 * Exported Function Prototypes
 *****************************************************************************/


#endif /* INTERFACE_CAN_MESSAGE_H_ */
