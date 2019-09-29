#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   16          //Multisampling

static const adc_atten_t atten = ADC_ATTEN_DB_0;
static esp_adc_cal_characteristics_t *adc_chars;

uint32_t adc_read(adc_unit_t unit, adc_channel_t channel);
