#ifndef MIC_H
#define MIC_H

#include "driver/i2s_std.h"

#define I2S_BUF_LEN 1024

extern i2s_chan_handle_t rx_handle;

void mic_init();
void mic_stream();

#endif