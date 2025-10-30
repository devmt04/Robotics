#include "../common/common.h"
#include "ir.h"
#include "driver/gpio.h"

int IR1_get_state(){
    return gpio_get_level(IRA);
}

int IR2_get_state(){
    return gpio_get_level(IRB);
}

