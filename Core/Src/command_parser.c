/*
 * Command parser: extracts !RST# and !OK# from the received characters.
 *
 * The UART interrupt only stores each character in a ring buffer
 * (buffer_put). The FSM below runs in the main loop and reads the
 * characters one by one:
 *
 *   WAIT_START --'!'--> RECEIVING --'#'--> check command, back to WAIT_START
 *                          |  other char: store it
 *                          |  '!'       : restart a new command
 *                          |  too long  : discard, back to WAIT_START
 */
#include <string.h>
#include "command_parser.h"

/* ring buffer written by the interrupt, read by the main loop */
static uint8_t buffer[MAX_BUFFER_SIZE];
static volatile uint8_t index_buffer = 0;   /* next position to write */
static uint8_t index_read = 0;              /* next position to read  */
volatile uint8_t buffer_flag = 0;

uint8_t command_flag = 0;
uint8_t command_data = CMD_NONE;

enum ParserState { WAIT_START, RECEIVING };
static enum ParserState parser_state = WAIT_START;
static char cmd[MAX_CMD_SIZE + 1];
static uint8_t cmd_len = 0;

void buffer_put(uint8_t c)
{
    buffer[index_buffer++] = c;
    if (index_buffer == MAX_BUFFER_SIZE) index_buffer = 0;
    buffer_flag = 1;
}

static void check_command(void)
{
    cmd[cmd_len] = '\0';
    if (strcmp(cmd, "RST") == 0) {
        command_data = CMD_RST;
        command_flag = 1;
    } else if (strcmp(cmd, "OK") == 0) {
        command_data = CMD_OK;
        command_flag = 1;
    }
    /* any other text between ! and # is ignored */
}

void command_parser_fsm(void)
{
    while (index_read != index_buffer) {          /* every new character */
        if (command_flag) {                       /* last command not used yet: */
            buffer_flag = 1;                      /* parse the rest next time   */
            return;
        }
        char c = (char)buffer[index_read++];
        if (index_read == MAX_BUFFER_SIZE) index_read = 0;

        switch (parser_state) {
        case WAIT_START:
            if (c == '!') {
                cmd_len = 0;
                parser_state = RECEIVING;
            }
            break;

        case RECEIVING:
            if (c == '!') {                       /* start again */
                cmd_len = 0;
            } else if (c == '#') {                /* end of command */
                check_command();
                parser_state = WAIT_START;
            } else if (cmd_len < MAX_CMD_SIZE) {
                cmd[cmd_len++] = c;
            } else {                              /* too long: discard */
                parser_state = WAIT_START;
            }
            break;
        }
    }
}
