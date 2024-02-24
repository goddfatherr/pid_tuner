#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

#include "inc/helper.h"

#ifdef CONFIG_USE_USER_CONF
#define ADC1_CHAN0          CONFIG_ADC1_CHAN0
#define ADC1_CHAN1          CONFIG_ADC1_CHAN1
#define ADC1_CHAN2          CONFIG_ADC1_CHAN2

#define GPIO_TACTILE_IN     CONFIG_GPIO_TACTILE

#else
#define ADC1_CHAN0          ADC_CHANNEL_0
#define ADC1_CHAN1          ADC_CHANNEL_3
#define ADC1_CHAN2          ADC_CHANNEL_6

#define GPIO_TACTILE_IN     13 
#endif



#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_TACTILE_IN))

#define ESP_INTR_FLAG_DEFAULT 0

#define ADC_ATTEN           ADC_ATTEN_DB_11

static const char *TAG = "pid_tuner_local";

int32_t P, I, D; 

adc_oneshot_unit_handle_t adc1_handle;

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR tactile_state_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
    
}


void setup_pot(){
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN1, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN2, &config));
}

void setup_tactile(){
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_TACTILE_IN, tactile_state_isr_handler, (void*) GPIO_TACTILE_IN);
}

static void get_pid_values(){
    int p, i, d; 
    
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN0, &p));
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN1, &i));
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN2, &d));
    
    P = (int32_t)p;
    I = (int32_t)i;
    D = (int32_t)d;

    ESP_LOGI(TAG, "P = %ld, I = %ld, D = %ld", P, I, D);
}

void task_set_pid_values(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            get_pid_values();
            persist_pid_values();
        }
    }
}

void start_local_pid_tuner(){
    setup_pot();
    setup_tactile();

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(task_set_pid_values, "pid_tuner", 2048, NULL, 10, NULL);
}

void stop_local_pid_tuner(){
    adc_oneshot_del_unit(adc1_handle);
    gpio_uninstall_isr_service();
    gpio_reset_pin(GPIO_TACTILE_IN);
}