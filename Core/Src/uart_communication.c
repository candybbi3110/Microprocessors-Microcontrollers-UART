/*
 * UART communication FSM (protocol of the lab):
 *
 *   IDLE      --!RST#-->  read ADC, send !ADC=xxxx#, start 3 s timer --> WAIT_OK
 *   WAIT_OK   --!OK#--->  stop timer                                  --> IDLE
 *   WAIT_OK   --3 s---->  send the SAME packet again, restart timer   --> WAIT_OK
 *   WAIT_OK   --!RST#-->  new request: read ADC again, send, restart  --> WAIT_OK
 */
#include <stdio.h>
#include "main.h"
#include "command_parser.h"
#include "software_timer.h"
#include "uart_communication.h"

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef  hadc1;

enum CommState { IDLE, WAIT_OK };
static enum CommState comm_state = IDLE;

static char packet[20];          /* last packet sent, kept for re-sending */
static int  packet_len = 0;

static uint32_t read_adc(void)
{
    return HAL_ADC_GetValue(&hadc1);   /* ADC runs in continuous mode */
}

/*
 * Send the packet by writing the UART data register directly.
 * HAL_UART_Transmit() locks the UART handle while it sends (about 10 ms
 * at 9600 bit/s); a character received during that time could not
 * restart the reception (HAL_UART_Receive_IT returned HAL_BUSY), so the
 * STM32 stopped receiving and never saw !OK#. Writing the registers
 * directly does not lock the handle, so the reception keeps working.
 */
static void send_packet(void)
{
    for (int i = 0; i < packet_len; i++) {
        while ((huart2.Instance->SR & USART_SR_TXE) == 0) { }   /* wait: data register empty */
        huart2.Instance->DR = (uint8_t)packet[i];
    }
    while ((huart2.Instance->SR & USART_SR_TC) == 0) { }       /* wait: last bit sent */
}

static void new_packet(void)
{
    uint32_t ADC_value = read_adc();
    packet_len = sprintf(packet, "!ADC=%lu#\r\n", (unsigned long)ADC_value);
    send_packet();
    setTimer(TIMER_TIMEOUT, ACK_TIMEOUT_MS);
}

void uart_communication_fsm(void)
{
    /* take the command produced by the parser, if any */
    uint8_t cmd = CMD_NONE;
    if (command_flag) {
        cmd = command_data;
        command_flag = 0;
    }

    switch (comm_state) {
    case IDLE:
        if (cmd == CMD_RST) {
            new_packet();
            comm_state = WAIT_OK;
        }
        /* !OK# without a request is ignored */
        break;

    case WAIT_OK:
        if (cmd == CMD_OK) {
            stopTimer(TIMER_TIMEOUT);
            comm_state = IDLE;
        } else if (cmd == CMD_RST) {
            new_packet();                     /* new request, new value */
        } else if (timer_flag[TIMER_TIMEOUT]) {
            send_packet();                    /* timeout: same packet again */
            setTimer(TIMER_TIMEOUT, ACK_TIMEOUT_MS);
        }
        break;
    }
}
