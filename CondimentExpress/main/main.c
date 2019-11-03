


#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "pwm.h"
#include "adc.h"
#include "hx711.h"
#include "gpio.h"
#include "pca9685.h"
#include "htu21d.h"

void hx711_test() 
{
    HX711 hx;
    hx.clockPin = 33;
    hx.dataPin = 32;
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

static char tag[] = "PCA9685";

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

static void i2c_example_master_init(void);

/**
 * @brief i2c master initialization
 */
static void i2c_example_master_init(void)
{
    ESP_LOGD(tag, ">> PCA9685");
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
    
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0));
}

void task_PCA9685(void *ignore)
{
    printf("Executing on core %d\n", xPortGetCoreID());

    esp_err_t ret;

    i2c_example_master_init();

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
    /*
        printf("Blink all pins starting from 0\n");

        for (uint8_t pin = 0; pin < 1; pin++)
        {
            printf("Turn LED %d on\n", pin);
            setPWM(pin, 4096, 0);

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

            vTaskDelay(100/portTICK_PERIOD_MS);

            printf("Turn LED %d off\n", pin);
            setPWM(pin, 0, 4096);
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
        */
    }
    //vTaskDelete(NULL);
}

void oled_main();

void app_main(void)
{
//    xTaskCreate(task_PCA9685, "task_PCA9685", 1024 * 2, (void* ) 0, 10, NULL);
//    task_PCA9685(NULL);
    // humidity_test();
    pwm_main();
    // oled_main();
//    printf("AAA");
//    GPIO_SetPin(14, GPIO_MODE_INPUT);
//    GPIO_SetPin(15, GPIO_MODE_INPUT);
//    GPIO_SetPin(26, GPIO_MODE_INPUT);
//    GPIO_SetPin(25, GPIO_MODE_INPUT);
}

