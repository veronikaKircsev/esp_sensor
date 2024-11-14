/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <esp_mac.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include "humidity_sensor.h"
#include "esp_task_wdt.h"
#include "temperature_sensor.h"
#include "freertos/semphr.h"
#include "data_types.h"
#include "udp_send.h"
#include "wifi_connection.h"
#include "blinker.h"

#define DATA_QUEUE_LENGTH 10
#define DATA_QUEUE_ITEM_SIZE sizeof(struct SensorData)

static char ip_address[16];
static bool ssid_found = false;

float temperature_celsius = 0.0f;
float humidity = 0.0f;

static QueueHandle_t dataQueueHandle = NULL;
static StaticQueue_t dataQueue;
static uint8_t dataQueueStorage[DATA_QUEUE_LENGTH * DATA_QUEUE_ITEM_SIZE];

TaskHandle_t humidity_task_handle = NULL;
TaskHandle_t temperature_task_handle = NULL;
TaskHandle_t udpHandle = NULL;
TaskHandle_t blinkHandle = NULL;
static bool wifi_connected = false;


struct SensorData sensorData = 
{
.temperature = 0, 
.humidity = 0,
.queue =  &dataQueueHandle, 
.type = NONE,
.ssid_found =  &ssid_found,
.wifi_connected = &wifi_connected};
void app_main(void)
{
    
    ESP_ERROR_CHECK(nvs_flash_init());
    
    configure_master_bus();
    configure_devices();
    configure_led();
    /*for debug*/
    //check_device();
    configure_temperature(); 
    configure_wifi();
    wifi_scan(&ssid_found);
    wifi_connect_to_network(&ip_address, &ssid_found, &wifi_connected);

    dataQueueHandle = xQueueCreateStatic(DATA_QUEUE_LENGTH, DATA_QUEUE_ITEM_SIZE, &dataQueueStorage, &dataQueue);
   
    xTaskCreate(read_sensor_data_humidity, "read_humidity", 4096, &sensorData, 1, &humidity_task_handle);
    configASSERT(humidity_task_handle);
    xTaskCreate(read_temperature, "read_temperature", 4096, &sensorData, 1, &temperature_task_handle);
    configASSERT(temperature_task_handle);
    xTaskCreate(send_udp_packet, "udp_sender", 4096, &sensorData, 1, &udpHandle);
    configASSERT( udpHandle );
    xTaskCreate(blinking, "blinking", 4096, NULL, 1, &blinkHandle);
    configASSERT( blinkHandle );
}