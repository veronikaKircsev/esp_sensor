
#include <stdio.h>
#include <esp_mac.h>
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "humidity_sensor.h"
#include "udp_send.h"
#include <stdint.h>
#include "data_types.h"
#include "esp_log.h"
#include "project_config.h"

static const char *MYTAG = "humidity_sensor";

#define SENSOR_I2C_PORT I2C_NUM_0
#define I2C_MASTER_SCL_IO 6
#define I2C_MASTER_SDA_IO 5


i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

float correction_factor_humidity = 1.5;


void configure_master_bus(void){

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = SENSOR_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .intr_priority = 0,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

void configure_devices(void){

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x70,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}


void read_sensor_data_humidity(void *pvParameters) {

    struct SensorData *sensor_data = (struct SensorData *)pvParameters;

    uint8_t data[4];
    esp_err_t ret;

    while (true)
    {
        if(sensor_data->wifi_connected){
        
        ret = i2c_master_receive(dev_handle, data, 4, -1);
        if (ret != ESP_OK) {
            ESP_LOGI(MYTAG, "Error reading from I2C device: %s", esp_err_to_name(ret));
            humidity_status = false;
        } else {
            sensor_data->humidity = ((((data[2] << 8 | data[3]) * 100.0 / 65535.0) * correction_factor_humidity));
            sensor_data->type = HUMIDITY;

            if(xQueueSend(*(sensor_data->queue),  sensor_data, 0) == pdFALSE){
                humidity_status = false;
            } else {
                humidity_status = true;
            }
        
            ESP_LOGI(MYTAG, "Humidity: %.2f%%", sensor_data->humidity);
        }
        }
        vTaskDelay(pdMS_TO_TICKS(10000));  // delay for 1 second
    }

}
/* only for debugging purposes
void check_device(void) {
    esp_err_t ret = i2c_master_probe(bus_handle, 0x70, -1);
    if (ret == ESP_OK) {
        printf("I2C device found at address 0x70");
    } else if (ret == ESP_ERR_NOT_FOUND) {
        printf("I2C device not found at address 0x70");
    } else {
        printf("I2C bus error: %s", esp_err_to_name(ret));
    }
}
*/