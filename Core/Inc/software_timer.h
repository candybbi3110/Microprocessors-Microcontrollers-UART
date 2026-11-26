#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define TIMER_CYCLE      10      /* timer resolution (ms)      */

#define TIMER_LED        0       /* LED blinky on PA5          */
#define TIMER_TIMEOUT    1       /* 3 s timeout waiting for !OK# */
#define NO_OF_TIMERS     2

extern volatile uint8_t timer_flag[NO_OF_TIMERS];

void setTimer(int index, int duration_ms);   /* start a timer            */
void stopTimer(int index);                   /* cancel a timer           */
void timerRun(void);                         /* call in the main loop    */

#endif /* INC_SOFTWARE_TIMER_H_ */
