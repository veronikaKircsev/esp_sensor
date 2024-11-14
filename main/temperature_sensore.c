#include <stdio.h>
#include "temperature_sensor.h"
#include "driver/temperature_sensor.h"
#include "data_types.h"
#include "esp_log.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "blinker.h"
#include "project_config.h"

static uint8_t correction_factor_temperature = 6;

static temperature_sensor_handle_t temp_handle;

const char *MTAG = "temperature_sensor";


void configure_temperature(void)
{
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(5, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_handle));
}

void read_temperature(void *pvParameters)
{
    struct SensorData *sensor_data = (struct SensorData *)pvParameters;
    esp_err_t ret;
    while (1) {
        
        if(sensor_data->wifi_connected){
            ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
            float temperature_celsius;
            ret = temperature_sensor_get_celsius(temp_handle, &temperature_celsius);
            if (ret != ESP_OK) {
                ESP_LOGE(MTAG, "Failed to read temperature sensor: %d", ret);
                temperature_status = false;
            } else {
                temperature_celsius -= correction_factor_temperature;
                sensor_data->temperature = temperature_celsius;
                sensor_data->type = TEMPERATURE;
                temperature_status = true;
                ESP_LOGI(MTAG, "Temperature: %.2f°C", temperature_celsius);
            }
            ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));

            if(xQueueSend(*(sensor_data->queue),  sensor_data, 0) == false){
                temperature_status = false;
            } else {
                temperature_status = true;
            }
            vTaskDelay(pdMS_TO_TICKS(10000));  // delay for 1 second
        }
    }

}