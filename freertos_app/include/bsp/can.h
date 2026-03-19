#ifndef CAN_H_
#define CAN_H_

#include "app.h"



#define RX_MAILBOX_CAN0  (0UL)
#define TX_MAILBOX_CAN0  (1UL)

#define Rx_Filter  0x0

void can_init(void);
void can_send(uint32_t id,uint8_t *data,uint8_t len);
void can_receive(uint32_t *id,uint8_t *data,uint8_t *len);

void  can_send_test(void);

#endif /* RTOS_H_ */
