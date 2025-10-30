#include "../common/common.h"
#include "engine.h"
#include "../motor/motors.h"
#include "../ir/ir.h" 
#include "../wifi/wifi.h"
#include "../http/http.h"
#include "mic/mic.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h" 

static void motor_task(){
    // gather all sensors info and move the robots
    int prev_dir = -1;
    motor_set_speed_A(15000);
    motor_set_speed_B(10000);
    motor_forward_A();
    motor_forward_B();
    while(1){
        size_t item_size = 0;
        // int *dir = (int *)xRingbufferReceive(motor_ringbuf, NULL, 0);
        int *ir = (int *)xRingbufferReceive(ir_ringbuf, &item_size, pdMS_TO_TICKS(10));
            //? maybe make it non-blocking
        if(ir){
            printf("state1 : %d state2 : %d \n", ir[0], ir[1]);
            vRingbufferReturnItem(ir_ringbuf, (void *)ir);
        }
        delayMS(350);
    }
}

static void ir_task(){
    // process and return combined states from IR sensors

    while(1){
        int ir[2] = {IR1_get_state(), IR2_get_state()};
        if(ir[0] == 0 || ir[1] == 0){
            UBaseType_t res =  xRingbufferSend(ir_ringbuf, &ir, sizeof(ir), pdMS_TO_TICKS(1000));
            if (res != pdTRUE) {
                printf("Failed to send item\n");
            }
            ESP_LOGI("IR", "Sending signal : IR1 - %d IR2 - %d", ir[0], ir[1]);
        }
        delayMS(350);
    }
}

void engine_init(){
    init_gpio();
    init_mcpwm();
    init_wifi();

    // The server won't wait for clients to connect, it will start it's server on port 8080
    init_ring_buf();
    mic_init();
    // http_server_init();

    xTaskCreatePinnedToCore(ir_task, "ir_task", 1024, NULL, 15, NULL, tskNO_AFFINITY); // allocate this 2048 bytes precisesly 
    xTaskCreatePinnedToCore(motor_task, "motor_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(mic_stream, "mic_stream_task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
}

