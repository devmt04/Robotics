#ifndef COMMON_H
#define COMMON_H

#include "esp_err.h"
#include "freertos/ringbuf.h"

#define delayMS(x) vTaskDelay(pdMS_TO_TICKS(x))

#define INA1    13
#define INA2    14
#define ENA     12     // PWM
#define INB1    19
#define INB2    20
#define ENB     21     // PWM

#define IRA     04
#define IRB     01

#define MIC_BCLK    47
#define MIC_WS      48
#define MIC_DIN     10

#define BDC_MCPWM_TIMER_RESOLUTION_HZ 10000000 // 10MHz, 1 tick = 0.1us
#define BDC_MCPWM_FREQ_HZ             25000    // 25KHz PWM
#define BDC_MCPWM_DUTY_TICK_MAX       (BDC_MCPWM_TIMER_RESOLUTION_HZ / BDC_MCPWM_FREQ_HZ) // maximum value we can set for the duty cycle, in ticks


extern RingbufHandle_t motor_ringbuf;
extern RingbufHandle_t ir_ringbuf;

void init_gpio();
void init_ring_buf();
// static void init_pwm();

#endif