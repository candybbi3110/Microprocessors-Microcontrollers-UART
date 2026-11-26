#ifndef INC_UART_COMMUNICATION_H_
#define INC_UART_COMMUNICATION_H_

#define ACK_TIMEOUT_MS   3000    /* wait at most 3 s for !OK# */

void uart_communication_fsm(void);   /* called in the main loop */

#endif /* INC_UART_COMMUNICATION_H_ */
