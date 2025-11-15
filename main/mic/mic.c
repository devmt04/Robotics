#include "../common/common.h"
#include "mic.h"
#include "driver/i2s_std.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"

i2s_chan_handle_t rx_handle = NULL;

static void enable_uart(){
    uart_config_t uart_config = {
        .baud_rate = 921600,     
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    // ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 0, 1024*sizeof(int32_t), 0, NULL, 0));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024*sizeof(int32_t), 0, 0, NULL, 0));
}

static void send_pcm(int32_t* buffer, size_t len) {
    // Send raw 16-bit PCM as bytes
    uart_write_bytes(UART_NUM_0, (const char*)buffer, len * sizeof(int32_t));
}

void mic_stream(){
    // int32_t buffer[1024];
    int32_t *buffer = malloc(I2S_BUF_LEN * sizeof(int32_t));
    if(!buffer){
        ESP_LOGE("MIC","Failed to allocate buffer");
        vTaskDelete(NULL);
        return;
    }
    size_t bytes_read = 0;
    while(1){
        ESP_ERROR_CHECK(i2s_channel_read(rx_handle, buffer, I2S_BUF_LEN * sizeof(int32_t), &bytes_read, 1000));

        int samples = bytes_read / sizeof(int32_t);
        // Print first few samples
        // for (int i = 0; i < samples; i++) {
        //     printf("%ld   -  %d  ,   %d\n", buffer[i], i, samples);
        // }
        // send_pcm(buffer, I2S_BUF_LEN * sizeof(int32_t));
        send_pcm(buffer, I2S_BUF_LEN);
        // delayMS(200);
    }
}

void mic_init(){
    i2s_chan_config_t channel_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&channel_config, NULL, &rx_handle));

    i2s_std_config_t std_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = MIC_BCLK,
            .ws = MIC_WS,
            .dout = I2S_GPIO_UNUSED,
            .din = MIC_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        }
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_config));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));

    ESP_LOGI("I2S", "I2S initilised successfully.");
    enable_uart();
}


