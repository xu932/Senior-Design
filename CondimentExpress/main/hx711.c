#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "hx711.h"
#include "gpio.h"

void HX711_Init(const HX711* hx) {
    gpio_pad_select_gpio(hx->dataPin);
    gpio_set_direction(hx->dataPin, GPIO_MODE_INPUT);

    gpio_pad_select_gpio(hx->clockPin);
    gpio_set_direction(hx->clockPin, GPIO_MODE_OUTPUT);
    //GPIO_SetPin(hx->dataPin, GPIO_MODE_INPUT);
    //GPIO_SetPin(hx->clockPin, GPIO_MODE_OUTPUT);
}

uint32_t HX711_Read(const HX711* hx) {
    while(!HX711_IsReady(hx))   vTaskDelay(1 / (portTICK_PERIOD_MS * 10));
    uint32_t data = 0x0;
    //uint32_t filter = 0x0;
    uint8_t i;
    for (i = 0; i < 24; i++) {
        data <<= 1;
        gpio_set_level(hx->clockPin, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        data |= gpio_get_level(hx->dataPin);
        gpio_set_level(hx->clockPin, 0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    for (i = 0; i < hx->gain; i++) {
        gpio_set_level(hx->clockPin, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        gpio_set_level(hx->clockPin, 0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    return data;
}

bool HX711_IsReady(const HX711* hx) {
    return gpio_get_level(hx->dataPin) == 0;
}

void HX711_PowerDown(const HX711* hx) {
    gpio_set_level(hx->clockPin, 0);
    gpio_set_level(hx->clockPin, 1);
}

void HX711_PowerUp(const HX711* hx) {
    gpio_set_level(hx->clockPin, 0);
}