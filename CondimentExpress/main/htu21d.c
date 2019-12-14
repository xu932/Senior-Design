#include "htu21d.h"
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

#define I2C_EXAMPLE_MASTER_SCL_IO   22    /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO   23    /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_FREQ_HZ  100000     /*!< I2C master clock frequency */
#define I2C_EXAMPLE_MASTER_NUM      I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */

#define HTU21D_ADDR 0x40
#define TEMPERATURE 0xE3
#define HUMIDITY    0xE5

#define ACK_CHECK_EN    0x1     /*!< I2C master will check ack from slave */


#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */


void soft_reset() {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTU21D_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0xfe, ACK_CHECK_EN);   // 0x0 = "Mode register 1"
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    vTaskDelay(50 / portTICK_RATE_MS);

    if (ret != ESP_OK) {
        printf("soft reset error\n");
    }
}

void read_temp() {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTU21D_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xF3, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    // wait for the sensor (50ms)
    vTaskDelay(50 / portTICK_RATE_MS);
    
    // receive the answer
    uint8_t msb, lsb, crc;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTU21D_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &msb, 0x00);
    i2c_master_read_byte(cmd, &lsb, 0x00);
    i2c_master_read_byte(cmd, &crc, 0x01);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        printf("read error\n");
    }

    uint16_t raw_value = ((uint16_t) msb << 8) | (uint16_t) lsb;
    float temperature = (raw_value * 175.72 / 65536.0) - 46.85;
    printf("temp: %f\n", temperature);
}

void read_reg() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (HTU21D_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0xe6, ACK_CHECK_EN);   // 0x0 = "Mode register 1"
    i2c_master_write_byte(cmd, (HTU21D_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN);   // 0x0 = "Mode register 1"

    uint8_t reg = 0x00;
    i2c_master_read_byte(cmd, &reg, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
    printf("reg: %x\n", reg);
    return;
}

void humidity_test() {
    read_reg();
    for (;;) {
        soft_reset();
        read_temp();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}