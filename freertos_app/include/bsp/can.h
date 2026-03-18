#ifndef CAN_H_
#define CAN_H_

#include "app.h"



#define RX_MAILBOX_CAN0  (0UL)
#define TX_MAILBOX_CAN0  (1UL)

#define Rx_Filter  0x0

void can_init(void);

#endif /* RTOS_H_ */
