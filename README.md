# Lab 5 – UART: Flow and Error Control (CO3010, HCMUT)

UART communication protocol on the **STM32F103C6**, built with STM32CubeIDE and simulated in Proteus.

- **Student:** Phạm Bùi Phương Trân – 2453286 – CC03
- **Course:** Microprocessors – Microcontrollers (CO3010)

## Protocol
| Step | Message |
|---|---|
| Request a sensor value | `!RST#` (from the terminal) |
| Reply | `!ADC=xxxx#` (from the STM32) |
| Acknowledge | `!OK#` (from the terminal) |
| No `!OK#` within 3 s | the same packet is sent again |

## Features
- USART2 at 9600 bit/s (8N1); the receive interrupt only stores each character in a ring buffer
- Two finite state machines in separate modules:
  - `command_parser_fsm()` extracts `!RST#` and `!OK#` from the received characters
  - `uart_communication_fsm()` reads the ADC, sends the packet and re-sends it after a 3 s timeout
- ADC1 channel 0 (PA0) reads a potentiometer through an op-amp voltage follower
- Software timers based on the HAL SysTick (no `HAL_Delay()` in the main loop); LED on PA5 blinks every 0.5 s

## Files
| File | Content |
|---|---|
| `Core/Src/command_parser.c`, `Core/Inc/command_parser.h` | Command parser FSM and ring buffer |
| `Core/Src/uart_communication.c`, `Core/Inc/uart_communication.h` | UART communication FSM (ADC reading, sending, timeout) |
| `Core/Src/software_timer.c`, `Core/Inc/software_timer.h` | Software timers |
| `Core/Src/main.c` | Initialisation, main loop, UART callbacks |
| `LAB5-UART.ioc` | STM32CubeMX configuration |
| `lab5_UART.pdsprj`, `LAB5-UART.hex` | Proteus project and compiled program |
