#include "common.h"
#include "driver/gpio.h"
#include "esp_log.h"

RingbufHandle_t motor_ringbuf = NULL;
RingbufHandle_t ir_ringbuf = NULL;

void init_gpio(){
    gpio_config_t motor_pin_conf = {
        .pin_bit_mask = (1ULL << INA1) | (1ULL << INA2) | (1ULL << INB1) | (1ULL << INB2 | (1ULL << ENA) | (1ULL << ENB) | (1ULL << IRA)| (1ULL << IRB)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = false,
        .pull_down_en = true,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&motor_pin_conf);

    gpio_config_t ir_pin_conf = {
        .pin_bit_mask = ((1ULL << IRA) | (1ULL << IRB)),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&ir_pin_conf);
}

void init_ring_buf(){
    // motor_ringbuf = xRingbufferCreate(1024, RINGBUF_TYPE_NOSPLIT);
    // if (motor_ringbuf == NULL) {
    //     ESP_LOGE("RING BUFFER", "Failed to create MOTOR ring buffer\n");
    // }

    ir_ringbuf = xRingbufferCreate(128, RINGBUF_TYPE_NOSPLIT);
    if(!ir_ringbuf){
         ESP_LOGE("RING BUFFER", "Failed tp  create IR ring buffer");
    }
}


// static void init_pwm(){
//     return;
// }
