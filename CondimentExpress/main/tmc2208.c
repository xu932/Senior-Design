#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "tmc2208.h"
#include "gpio.h"
#include "sx1509.h"
#include "ssd1306.h"

static ledc_channel_config_t ledc_channel[PWM_CH_NUM];
static uint32_t xPos = 0;
static uint32_t yPos = 0;

static uint8_t sign(int a) {return a > 0 ? 1 : 0;}

static uint32_t min(uint32_t a, uint32_t b) {return a < b ? a : b;}

void motor_xy_init() {
    GPIO_SetPin(XY_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(XY_EN, 1);
    init_output_pin(XY_1_DIR);
    init_output_pin(XY_2_DIR);

    // gpio_set_level(XY_1_DIR, 0);
    // gpio_set_level(XY_1_EN, 1);

    // GPIO_SetPin(XY_2_DIR, GPIO_MODE_OUTPUT);
    // GPIO_SetPin(XY_2_EN, GPIO_MODE_OUTPUT);

    // gpio_set_level(XY_2_DIR, 0);
    // gpio_set_level(XY_2_EN, 1);

    init_output_pin(LIQ_EN1);
    init_output_pin(LIQ_EN2);
    init_output_pin(LIQ_EN3);
    set_output_level(LIQ_EN1, 1);
    set_output_level(LIQ_EN2, 1);
    set_output_level(LIQ_EN3, 1);

    GPIO_SetPin(LIQ_DIR, GPIO_MODE_OUTPUT);
    gpio_set_level(LIQ_DIR, 1);
}

void motor_xy_home() {
    // TODO
    xPos = 0.0;
    yPos = 0.0;
}

void _motor_xy_move(uint32_t xy_1_dir, uint32_t xy_1_time, uint32_t xy_2_dir, uint32_t xy_2_time) {
    gpio_set_level(XY_1_DIR, xy_1_dir);
    gpio_set_level(XY_2_DIR, xy_2_dir);
    gpio_set_level(XY_EN, 0);
    gpio_set_level(XY_EN, 0);
    
    if (xy_1_time < xy_2_time) {
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);

        vTaskDelay(xy_1_time / portTICK_PERIOD_MS);
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);

        vTaskDelay((xy_2_time - xy_1_time) / portTICK_PERIOD_MS);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, 0);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
    } else {
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
    
        vTaskDelay(xy_2_time / portTICK_PERIOD_MS);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, 0);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);

        vTaskDelay((xy_1_time - xy_2_time) / portTICK_PERIOD_MS);
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
    }

    gpio_set_level(XY_EN, 1);
    gpio_set_level(XY_EN, 1);
}

void motor_xy_move(uint32_t newX, uint32_t newY) {
    int dx = newX - xPos;
    int dy = newY - yPos;
    int move1 = (dx + dy) / 2;
    int move2 = (dy - dx) / 2;

    _motor_xy_move(sign(move1), abs(move1), sign(move2), abs(move2));

    xPos = newX;
    yPos = newY;
}

void pwm_init()
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 400,                       // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,           // timer mode
        .timer_num = LEDC_TIMER_0,           // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };

    ledc_timer_config(&ledc_timer);

    ledc_timer.freq_hz = 400;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_1;
    ledc_timer_config(&ledc_timer);

    ledc_channel[0].channel    = LEDC_CHANNEL_0;
    ledc_channel[0].duty       = 0;
    ledc_channel[0].gpio_num   = XY_1_PWM;
    ledc_channel[0].speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel[0].hpoint     = 0;
    ledc_channel[0].timer_sel  = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel[0]);

    ledc_channel[1].channel    = LEDC_CHANNEL_1;
    ledc_channel[1].duty       = 0;
    ledc_channel[1].gpio_num   = XY_2_PWM;
    ledc_channel[1].speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel[1].hpoint     = 0;
    ledc_channel[1].timer_sel  = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel[1]);

    ledc_channel[2].channel    = LEDC_CHANNEL_2;
    ledc_channel[2].duty       = 0;
    ledc_channel[2].gpio_num   = LIQ_PWM;
    ledc_channel[2].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[2].hpoint     = 0;
    ledc_channel[2].timer_sel  = LEDC_TIMER_1;
    ledc_channel_config(&ledc_channel[2]);

    ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, LEDC_TEST_DUTY);
    ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
}

void move_motor(int i) {

    switch(i) {
        case 0:
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        gpio_set_level(XY_EN, 1);
        printline("pump stop", 9);
        break;
        case 1:
        gpio_set_level(XY_EN, 0);
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, LEDC_TEST_DUTY);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        printline("pump move", 9);
        break;
    }
    // switch(i) {
    //     case 1:
    //     set_output_level(LIQ_EN1, 0);
    //     vTaskDelay(300);
    //     set_output_level(LIQ_EN1, 1);
    //     break;
    //     case 2:
    //     set_output_level(LIQ_EN2, 0);
    //     vTaskDelay(300);
    //     set_output_level(LIQ_EN2, 1);
    //     break;
    //     case 3:
    //     set_output_level(LIQ_EN3, 0);
    //     vTaskDelay(300);
    //     set_output_level(LIQ_EN3, 1);
    //     break;
    // }
    // ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, 0);
    // ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);

}

void _testpump(int i) {
    char s[20] = {0};
    sprintf(s, "pump %d", i);
    print_at_line(s, strlen(s), 1);

    // ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, LEDC_TEST_DUTY);
    // ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
    
    switch(i) {
        case 0:
        set_output_level(LIQ_EN1, 1);
        set_output_level(LIQ_EN3, 1);
        printline("motor stop", 9);
        break;
        case 2:
        gpio_set_level(LIQ_DIR, 0);
        set_output_level(LIQ_EN1, 0);
        printline("motor move", 9);
        break;
        case 3:
        gpio_set_level(LIQ_DIR, 1);
        set_output_level(LIQ_EN1, 0);
        printline("motor move", 9);
        break;
        case 4:
        gpio_set_level(LIQ_DIR, 0);
        set_output_level(LIQ_EN3, 0);
        printline("motor move", 9);
        break;
        case 5:
        gpio_set_level(LIQ_DIR, 1);
        set_output_level(LIQ_EN3, 0);
        printline("motor move", 9);
        break;
    }
}

void _testmove(int i) {
    
    move_motor(i);
}
