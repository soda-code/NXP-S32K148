/*
 * can_fd_message.h
 *
 *  Created on: 2026年5月23日
 *      Author: Administrator
 */

#ifndef INTERFACE_CAN_FD_MESSAGE_H_
#define INTERFACE_CAN_FD_MESSAGE_H_


/*****************************************************************************
 *  Defines
 *****************************************************************************/
#define INVALID_CAN_ID 0xFFFFFFFF
/*****************************************************************************
 *  Data Types
 *****************************************************************************/
#ifndef DEFINED_TYPEDEF_FOR_CAN_FD_MESSAGE_BUS_
#define DEFINED_TYPEDEF_FOR_CAN_FD_MESSAGE_BUS_

typedef struct {
  uint8_t ProtocolMode;
  uint8_t Extended;
  uint8_t Length;
  uint8_t Remote;
  uint8_t Error;
  uint8_t BRS;
  uint8_t ESI;
  uint8_t DLC;
  uint32_t ID;
  uint32_t Reserved;
  float Timestamp;
  uint8_t Data[64];
} CAN_FD_MESSAGE_BUS;

#endif

#ifndef DEFINED_TYPEDEF_FOR_CAN_MESSAGE_BUS_
#define DEFINED_TYPEDEF_FOR_CAN_MESSAGE_BUS_

typedef struct {
  uint8_t Extended;
  uint8_t Length;
  uint8_t Remote;
  uint8_t Error;
  uint32_t ID;
  float Timestamp;
  uint8_t Data[8];
} CAN_MESSAGE_BUS;

#endif

#endif /* INTERFACE_CAN_FD_MESSAGE_H_ */
