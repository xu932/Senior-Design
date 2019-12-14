


#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "tmc2208.h"
#include "hx711.h"
#include "hx711_2.h"
#include "gpio.h"
#include "pca9685.h"
#include "htu21d.h"
#include "sx1509.h"
#include "ssd1306.h"

void hx711_test() 
{
    HX711 hx;
    hx.dataPin = 23;
    hx.clockPin = 22;
    uint32_t read;
    HX711_Init(&hx);
    HX711_PowerUp(&hx);
    printf("Initialized\n");
    for (;;) {
        
        read = HX711_Read(&hx);
        printf("%d\n", read);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#define I2C_EXAMPLE_MASTER_SCL_IO   22    /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO   23    /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_FREQ_HZ  100000     /*!< I2C master clock frequency */
#define I2C_EXAMPLE_MASTER_NUM      I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */


#define I2C_ADDRESS     0x40    /*!< lave address for PCA9685 */

#define ACK_CHECK_EN    0x1     /*!< I2C master will check ack from slave */
#define ACK_CHECK_DIS   0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL         0x0     /*!< I2C ack value */
#define NACK_VAL        0x1     /*!< I2C nack value */

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);


void task_PCA9685(void *ignore)
{
    printf("Executing on core %d\n", xPortGetCoreID());

    esp_err_t ret;

    set_pca9685_adress(I2C_ADDRESS);
    resetPCA9685();
    setFrequencyPCA9685(1000);  // 1000 Hz
    turnAllOff();

    printf("Finished setup, entering loop\n");

    while(1)
    {
        // fade up and down each pin with static logarithmic table
        // see Weber Fechner Law
        ret = fade_pin_up_down(0);
        

        if(ret == ESP_ERR_TIMEOUT)
        {
            printf("I2C timeout\n");
        }
        else if(ret == ESP_OK)
        {
            // all good
        }
        else
        {
            printf("No ack, sensor not connected...skip...\n");
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    //vTaskDelete(NULL);
}

void oled_main();

void test_beep() {
    init_mux();
    printf("before: \n");
    _mux_test_read();
    init_output_pin(6);
    set_output_level(6, 0);
    printf("after: \n");
    _mux_test_read();
    // while(true) {
    //     set_output_level(6, 0);
    //     vTaskDelay(1);
    //     set_output_level(6, 1);
    //     vTaskDelay(1);
    // }
}

void init_i2c() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 23;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode,
                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0));
}

void ble_main();
void lcd_main();

#define AVG_SAMPLES 10
#define GPIO_DATA   14
#define GPIO_SCLK   12

void hx711_2_init() {
    HX711_init(GPIO_DATA, GPIO_SCLK, eGAIN_128); 
    HX711_tare();
}

void hx711_2_measure() {
    unsigned long weight = HX711_read_average(AVG_SAMPLES);
    char s[22] = {0};
    sprintf(s, "weight %ld", weight);
    printf("%s\n", s);
    print_at_line(s, strlen(s), 0);
}

void process_command(const char* command, int len) {
    if (command[0] == 'm' && command[1] == 'e' && command[2] == 'a') {
        hx711_2_measure();
    } else if (command[0] == 'p' && command[1] == 'u' && command[2] == 'm' && command[3] == 'p') {
        int p;
        sscanf(command, "pump %d", &p);
        _testmove(p);
    } else if (command[0] == 'm' && command[1] == 'o' && command[2] == 'v' && command[3] == 'e') {
        int m;
        sscanf(command, "move %d", &m);
        _testpump(m);
    } else {
        print_at_line("command error", 13, 0);
    }
}

void app_main(void)
{
    init_i2c();
    init_mux();
    hx711_2_init();
    motor_xy_init();
    pwm_init();
    lcd_main();
    ble_main();



    // test_beep();

    // printf("%d\n", portTICK_RATE_MS);
    /*
    motor_xy_init();
    pwm_init();
    motor_xy_move(1750, 3500);
    vTaskDelay(100);
    motor_xy_move(3500, 1700);
    vTaskDelay(100);
    motor_xy_move(1750, 0);
    vTaskDelay(100);
    motor_xy_move(0, 3500);
    printf("Done!\n");
    */
    //_testmove(0);
    //3500 diagonal
    //pwm_move(0, 1, 3500);
//    xTaskCreate(task_PCA9685, "task_PCA9685", 1024 * 2, (void* ) 0, 10, NULL);
//    task_PCA9685(NULL);
    // humidity_test();
    // pwm_main();

    // printf("ready!\n");
    // printf("3!\n");
    // vTaskDelay(100);
    // printf("2!\n");
    // vTaskDelay(100);
    // printf("1!\n");
    // vTaskDelay(100);
    // oled_main();

//    printf("AAA");
//    GPIO_SetPin(14, GPIO_MODE_INPUT);
//    GPIO_SetPin(15, GPIO_MODE_INPUT);
//    GPIO_SetPin(26, GPIO_MODE_INPUT);
//    GPIO_SetPin(25, GPIO_MODE_INPUT);
}