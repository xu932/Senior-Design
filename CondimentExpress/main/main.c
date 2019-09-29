
#include <stdio.h>
#include "pwm.h"
#include "adc.h"
#include "hx711.h"

void app_main(void)
{
    HX711 hx;
    hx.clockPin = 14;
    hx.dataPin = 15;
    uint32_t read;
    HX711_Init(&hx);
    HX711_PowerUp(&hx);
    printf("Initialized\n");
    for (;;) {
        
        read = HX711_Read(&hx);
        printf("%d\n", read);
        vTaskDelay(pdMS_TO_TICKS(1000));
/*
        gpio_set_level(hx.clockPin, 1);
        vTaskDelay(20 / portTICK_PERIOD_MS);
        //data |= gpio_get_level(hx->dataPin);
        gpio_set_level(hx.clockPin, 0);
        vTaskDelay(20 / portTICK_PERIOD_MS);
        */
    }
}

