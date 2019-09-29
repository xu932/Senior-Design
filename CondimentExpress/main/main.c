
#include <stdio.h>
#include "pwm.h"
#include "adc.h"

void app_main(void)
{
    for (;;) {
        uint32_t val = adc_read(ADC_UNIT_2, ADC_CHANNEL_6);
        printf("%d\n", val);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
–