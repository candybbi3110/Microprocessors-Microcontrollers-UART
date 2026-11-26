#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_

#include <stdint.h>

#define MAX_BUFFER_SIZE   30     /* ring buffer for received characters */
#define MAX_CMD_SIZE      10     /* longest command between ! and #     */

/* commands recognised by the parser */
#define CMD_NONE   0
#define CMD_RST    1             /* !RST#  : request a sensor value     */
#define CMD_OK     2             /* !OK#   : acknowledge                */

extern volatile uint8_t buffer_flag;     /* 1 = new character(s) received */
extern uint8_t command_flag;             /* 1 = a command is ready        */
extern uint8_t command_data;             /* CMD_RST or CMD_OK             */

void buffer_put(uint8_t c);              /* called in the UART RX callback */
void command_parser_fsm(void);           /* called in the main loop        */

#endif /* INC_COMMAND_PARSER_H_ */
