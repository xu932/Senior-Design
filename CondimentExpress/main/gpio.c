#include "gpio.h"

void GPIO_SetPin(uint64_t pins, gpio_mode_t mode) {
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = mode;
    io_conf.pin_bit_mask = (1ULL) << pins;
    if (mode == GPIO_MODE_INPUT) {
        io_conf.pull_up_en = 1;
    }
    gpio_config(&io_conf);
}