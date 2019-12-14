#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"

#include "sx1509.h"
#include "sx1509_reg.h"

#define MUX_ADDR 0x3e

static uint16_t directions = 0xffff;
static uint16_t pin_data = 0xffff;

void writeByte(uint8_t reg_addr, uint8_t val) {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MUX_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, val, 0);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        printf("mux error\n");
    }
}

void reset_mux() {
    writeByte(REG_RESET, 0x12);
    writeByte(REG_RESET, 0x34);
}

void init_mux() {
    reset_mux();
    printf("init done\n");
}

void init_output_pin(uint8_t pin) {
    directions &= ~(0x1 << pin);
    writeByte(REG_DIR_A, directions & 0xff);
    writeByte(REG_DIR_B, (directions >> 8) & 0xff);
}

void set_output_level(uint8_t pin, bool on) {
    if (on) {   // set high
        printf("pin %d set high: %x\n", pin, pin_data & 0xffff);
        pin_data |= 0x1 << pin;
    } else {    // set low
        printf("pin %d set low: %x\n", pin, pin_data & 0xffff);
        pin_data &= ~(0x1 << pin);
    }
    writeByte(REG_DATA_A, pin_data & 0xff);
    writeByte(REG_DATA_B, (pin_data >> 8) & 0xff);
}

uint8_t readByte(uint8_t reg) {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MUX_ADDR << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, reg, 0);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    uint8_t msb = 0x55;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MUX_ADDR << 1) | I2C_MASTER_READ, 1);
    i2c_master_read_byte(cmd, &msb, 0x00);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        printf("read error\n");
    }

    return msb;
}

void _mux_test_read() {
    uint16_t dir_read = 0x0, data_read = 0x0;
    dir_read |= readByte(REG_DIR_B);
    dir_read <<= 8;
    dir_read |= readByte(REG_DIR_A);
    vTaskDelay(100 / portTICK_RATE_MS);
    data_read |= readByte(REG_DATA_B);
    data_read <<= 8;
    data_read |= readByte(REG_DATA_A);
    printf("dir:  %x\ndata: %x\n", dir_read, data_read);
}