#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/i2c.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "esp_err.h"
#include <errno.h>
#include "esp_log.h"
#include "esp_system.h"
#include "ssd1305.h"
#include "gpio.h"

#define I2C_EXAMPLE_MASTER_SCL_IO   22    /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO   23    /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_FREQ_HZ  100000     /*!< I2C master clock frequency */
#define I2C_EXAMPLE_MASTER_NUM      I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */

static uint8_t letters[27][6] = {
    {0x00, 0xfc, 0x22, 0x22, 0xfc, 0x00}, {0x00, 0xfe, 0x92, 0x92, 0x7c, 0x00}, {0x00, 0x7c, 0x82, 0x82, 0x44, 0x00},   // ABC
    {0x00, 0xfe, 0x82, 0x82, 0x7c, 0x00}, {0x00, 0xfe, 0x92, 0x92, 0x82, 0x00}, {0x00, 0xfe, 0x12, 0x12, 0x02, 0x00},   // DEF
    {0x00, 0x7c, 0x82, 0xa2, 0x64, 0x00}, {0x00, 0xfe, 0x10, 0x10, 0xfe, 0x00}, {0x00, 0x82, 0xfe, 0x82, 0x00, 0x00},   // GHI
    {0x00, 0x60, 0x80, 0x80, 0x7e, 0x00}, {0x00, 0xfe, 0x10, 0x28, 0xc6, 0x00}, {0x00, 0xfe, 0x80, 0x80, 0x80, 0x00},   // JKL
    {0x00, 0xfe, 0x04, 0x08, 0x04, 0xfe}, {0x00, 0xfe, 0x08, 0x10, 0xfe, 0x00}, {0x00, 0x7c, 0x82, 0x82, 0x7c, 0x00},   // MNO
    {0x00, 0xfe, 0x12, 0x12, 0x0c, 0x00}, {0x00, 0x7c, 0x82, 0x42, 0xbc, 0x00}, {0x00, 0xfe, 0x12, 0x12, 0xec, 0x00},   // PQR
    {0x00, 0x4c, 0x92, 0x92, 0x64, 0x00}, {0x00, 0x02, 0x02, 0xfe, 0x02, 0x02}, {0x00, 0x7e, 0x80, 0x80, 0x7e, 0x00},   // STU
    {0x00, 0x3e, 0x40, 0x80, 0x40, 0x3e}, {0x00, 0x7e, 0x80, 0x60, 0x80, 0x7e}, {0x00, 0xee, 0x10, 0x10, 0xee, 0x00},   // VWX
    {0x00, 0x06, 0x08, 0xf0, 0x08, 0x06}, {0x00, 0xe2, 0x92, 0x92, 0x8e, 0x00}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // YZ\s
};

static uint8_t buffer[SSD1305_LCDHEIGHT * SSD1305_LCDWIDTH / 8] = { 
// page 1
0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 2
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 3
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 4
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 5
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 6
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 7
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// page 8
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void command(uint8_t data) {
//    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1305_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    //ret = 
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

void begin() {
    GPIO_SetPin(SSD1305_RST_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SSD1305_RST_GPIO, 1);
    vTaskDelay(1 / portTICK_RATE_MS);
    gpio_set_level(SSD1305_RST_GPIO, 0);
    vTaskDelay(10 / portTICK_RATE_MS);
    gpio_set_level(SSD1305_RST_GPIO, 1);

    command(SSD1305_DISPLAYOFF);                    // 0xAE
    command(SSD1305_SETLOWCOLUMN | 0x4);  // low col = 0
    command(SSD1305_SETHIGHCOLUMN | 0x4);  // hi col = 0
    command(SSD1305_SETSTARTLINE | 0x0); // line #0
    command(0x2E); //??
    command(SSD1305_SETCONTRAST);                   // 0x81
    command(0x32);
    command(SSD1305_SETBRIGHTNESS);                 // 0x82
    command(0x80);
    command(SSD1305_SEGREMAP | 0x01);
    command(SSD1305_NORMALDISPLAY);                 // 0xA6
    command(SSD1305_SETMULTIPLEX);                  // 0xA8
    command(0x3F); // 1/64
    command(SSD1305_MASTERCONFIG);
    command(0x8e); /* external vcc supply */
    command(SSD1305_COMSCANDEC);
    command(SSD1305_SETDISPLAYOFFSET);              // 0xD3
    command(0x40); 
    command(SSD1305_SETDISPLAYCLOCKDIV);            // 0xD5
    command(0xf0); 
    command(SSD1305_SETAREACOLOR);  
    command(0x05);
    command(SSD1305_SETPRECHARGE);                  // 0xd9
    command(0xF1);
    command(SSD1305_SETCOMPINS);                    // 0xDA
    command(0x12);

    command(SSD1305_SETLUT);
    command(0x3F);
    command(0x3F);
    command(0x3F);
    command(0x3F);

    command(SSD1305_DISPLAYON);//--turn on oled panel
}

void display() {
    uint32_t i = 0;
    uint8_t page = 0;
    i2c_cmd_handle_t cmd;
    
    for(; page < 8; page++) {
        command(SSD1305_SETPAGESTART + page);
        command(0x00);
        command(0x10);
        for (uint8_t w=0; w < 128 / 8; w++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (SSD1305_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, 0x40, true);
            for (uint8_t x=0; x<8; x++) {
                i2c_master_write_byte(cmd, buffer[i++], true);
            }
            i2c_master_stop(cmd);
            i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
        }
    }

    // i = 0;
    // uint8_t j = 0;
    // for (page = 0; page < 2; page++) {
    //     command(SSD1305_SETPAGESTART + page);
    //     command(0x00);
    //     command(0x10);
    //     j = 0;
    //     while (j < 13) {
    //         cmd = i2c_cmd_link_create();
    //         i2c_master_start(cmd);
    //         i2c_master_write_byte(cmd, (SSD1305_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    //         i2c_master_write_byte(cmd, 0x40, true);
    //         for (uint8_t x=0; x<6; x++) {
    //             i2c_master_write_byte(cmd, letters[i][x], true);
    //         }
    //         i2c_master_stop(cmd);
    //         i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
    //         i2c_cmd_link_delete(cmd);
    //         i++;
    //         j++;
    //     }
    // }
}

void clear() {
    uint8_t page = 0;
    i2c_cmd_handle_t cmd;
    
    for(; page < 8; page++) {
        command(SSD1305_SETPAGESTART + page);
        command(0x00);
        command(0x10);
        for (uint8_t w=0; w < 128 / 8; w++) {
            cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (SSD1305_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, 0x40, true);
            for (uint8_t x=0; x<8; x++) {
                i2c_master_write_byte(cmd, 0x00, true);
            }
            i2c_master_stop(cmd);
            i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
        }
    }
}

void printline(int page, char* str, int len) {
    uint8_t i = 0;
    uint8_t j = 0;
    i2c_cmd_handle_t cmd;

    command(SSD1305_SETPAGESTART + page);
    command(0x00);
    command(0x10);
    while (j < len && j < 21 && str[j] != '\0') {
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (SSD1305_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, 0x40, true);
        i = str[j] == ' ' ? 26 : str[j] - 'A';
        for (uint8_t x=0; x<6; x++) {
            i2c_master_write_byte(cmd, letters[i][x], true);
        }
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        j++;
    }
}

void oled_main() {
    begin();
    clear();
    // display();
    //printline(1, "HELLO WORLD", 11);
    //printline(7, "HELLO WORLD", 11);
    /*
    for (int i = 0; i < SSD1305_LCDHEIGHT * SSD1305_LCDWIDTH / 8; i++) {
        for (int j = 0; j < 8; j++) {
            buffer[i] |= 0x1 << j;
            display();
            vTaskDelay(3000 / portTICK_RATE_MS);
        }
    }*/
}