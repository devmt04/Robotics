#include "driver/gpio.h"
#include "../common/common.h"
#include "motors.h"
#include "freertos/FreeRTOS.h"
#include "driver/mcpwm_prelude.h"

// Rotatring pricniple : Slowly rorate both tyres in reverse directions
mcpwm_cmpr_handle_t comparator_handle_A;
mcpwm_cmpr_handle_t comparator_handle_B;
    
void motor_break_A(){
    gpio_set_level(INA1, 0);
    gpio_set_level(INA2, 0);
}

void motor_break_B(){
    gpio_set_level(INB1, 0);
    gpio_set_level(INB2, 0);
}

void motor_forward_A(){
    gpio_set_level(INA1, 0);
    gpio_set_level(INA2, 1);
}

void motor_backward_A(){
    gpio_set_level(INA1, 1);
    gpio_set_level(INA2, 0);
}

void motor_forward_B(){
    gpio_set_level(INB1, 0);
    gpio_set_level(INB2, 1);
}

void motor_backward_B(){
    gpio_set_level(INB1, 1);
    gpio_set_level(INB2, 0);
}

void motor_set_speed_A(int duty_ticks){
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator_handle_A, duty_ticks));
}

void motor_set_speed_B(int duty_ticks){
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator_handle_B, duty_ticks));
}

void init_mcpwm(){
    mcpwm_timer_handle_t timer_handle;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = BDC_MCPWM_FREQ_HZ,
        // .intr_priority =,
        // .update_period_on_empty =,
        // .update_period_on_sync =,
        // .allow_pd =,
        // .flags ={}
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer_handle));

    mcpwm_oper_handle_t operator_handle;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
        // .intr_priority =,
        // .update_gen_action_on_tez =,
        // .update_gen_action_on_tep = ,
        // .update_gen_action_on_sync =,
        // .update_gen_action_on_sync =,
        // .update_dead_time_on_tep =,
        // .update_dead_time_on_sync =,
        // .flags = {}
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operator_handle));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operator_handle, timer_handle));

    mcpwm_comparator_config_t comparator_config = {
        // .intr_priority = ,
        // .update_cmp_on_tez = ,
        // .update_cmp_on_tep =,
        // .update_cmp_on_sync =,
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(operator_handle, &comparator_config, &comparator_handle_A));
    ESP_ERROR_CHECK(mcpwm_new_comparator(operator_handle, &comparator_config, &comparator_handle_B));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator_handle_A, 0));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator_handle_B, 0));
    
    mcpwm_gen_handle_t generator_handle_A;
    mcpwm_generator_config_t generator_congif_A = {
        .gen_gpio_num = ENA,
        // .invert_pwm =,
        // .io_loop_back =,
        // .io_od_mode =,
        // .pull_up =,
        // .pull_down =,
        // .flags = {},
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(operator_handle, &generator_congif_A, &generator_handle_A));
    
    mcpwm_gen_handle_t generator_handle_B;
    mcpwm_generator_config_t generator_congif_B = {
        .gen_gpio_num = ENB,
        // .invert_pwm =,
        // .io_loop_back =,
        // .io_od_mode =,
        // .pull_up =,
        // .pull_down =,
        // .flags = {},
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(operator_handle, &generator_congif_B, &generator_handle_B));
    
    mcpwm_generator_set_action_on_timer_event(generator_handle_A,
            MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(generator_handle_A,
            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator_handle_A, MCPWM_GEN_ACTION_LOW));
    mcpwm_generator_set_action_on_timer_event(generator_handle_B,
            MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(generator_handle_B,
            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator_handle_B, MCPWM_GEN_ACTION_LOW));

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer_handle));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer_handle, MCPWM_TIMER_START_NO_STOP));
    


    // err:
        // cleanup here

}

// static void motor_task(){
//     // gather all sensors info and move the robots
//     int prev_dir = -1;
//     while(1){
//         size_t item_size;
//         int *dir = (int *)xRingbufferReceive(motor_ringbuf, &item_size, pdMS_TO_TICKS(1000));
//         if(dir != NULL){
//             int direction = *dir;
//             if(direction == 0){
//                 if(direction != prev_dir){
//                     motor_break_A();
//                     motor_break_B();
//                     delayMS(100);
//                 }
//                 motor_forward_A();
//                 motor_forward_B();
//                 prev_dir = direction;
//             }else if(direction == 1){
//                 if(direction != prev_dir){
//                     motor_break_A();
//                     motor_break_B();
//                     delayMS(100);
//                 }
//                 motor_forward_A();
//                 motor_break_B();
//                 prev_dir = direction;
//             }else if(direction == 2){
//                 if(direction != prev_dir){
//                     motor_break_A();
//                     motor_break_B();
//                     delayMS(100);
//                 }
//                 motor_backward_A();
//                 motor_backward_B();
//                 prev_dir = direction;
//             }else if(direction == 3){
//                 if(direction != prev_dir){
//                     motor_break_A();
//                     motor_break_B();
//                     delayMS(100);
//                 }
//                 motor_break_A();;
//                 motor_forward_B();
//                 prev_dir = direction;
//             }else if(direction == 4){
//                 motor_break_A();
//                 motor_break_B();
//                 prev_dir = direction;
//             }
//             vRingbufferReturnItem(motor_ringbuf, (void *)dir);
//         }
//         delayMS(250);
//     }
// }

// static void motor_event_loop(){
    
// }

// void motor_task_init(){
//     xTaskCreatePinnedToCore(motor_event_loop, "motor_event", 4096, NULL, 10, NULL, tskNO_AFFINITY);
// }

// static void motor_set_speed_A(){
//     return;
// }

// static void motor_set_speed_B(){
//     return;
// }