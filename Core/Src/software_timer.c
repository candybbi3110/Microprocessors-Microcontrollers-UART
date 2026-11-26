/*
 * Software timers counted from the HAL SysTick (HAL_GetTick, 1 ms).
 * timerRun() is called in the main loop: every 10 ms that has passed,
 * each running timer is decremented; when it reaches 0 its flag is set.
 */
#include "main.h"
#include "software_timer.h"

static int timer_counter[NO_OF_TIMERS];
volatile uint8_t timer_flag[NO_OF_TIMERS];

void setTimer(int index, int duration_ms)
{
    int ticks = duration_ms / TIMER_CYCLE;
    if (ticks <= 0) ticks = 1;
    timer_flag[index] = 0;
    timer_counter[index] = ticks;
}

void stopTimer(int index)
{
    timer_counter[index] = 0;
    timer_flag[index] = 0;
}

void timerRun(void)
{
    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();                    /* ms since start-up */

    while ((uint32_t)(now - last_tick) >= TIMER_CYCLE) {   /* every 10 ms */
        last_tick += TIMER_CYCLE;
        for (int i = 0; i < NO_OF_TIMERS; i++) {
            if (timer_counter[i] > 0) {
                timer_counter[i]--;
                if (timer_counter[i] == 0) timer_flag[i] = 1;
            }
        }
    }
}
