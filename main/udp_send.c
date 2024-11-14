
#include <stdio.h>
#include <string.h>
#include <esp_mac.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp_send.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "data_types.h"
#include "project_config.h"

#define TAGS "UDP_SENDER"

void send_udp_packet(void *pvParameters) {
    struct SensorData *sensorData = (struct SensorData *)pvParameters;

    while (1) {
        if (xQueueReceive(*(sensorData->queue), sensorData, pdMS_TO_TICKS(1000)) == pdTRUE) {

            uint8_t raw_data[6];  // 1 byte for device_id, 1 byte for type, 4 bytes for float value
            
            raw_data[0] = device_id;
            float value = 0.0f;
            
            if (sensorData->type == TEMPERATURE) {
                raw_data[1] = 't';
                value = sensorData->temperature;
                ESP_LOGI(TAGS, "Temperature in send: %.2f", value);
            } else if (sensorData->type == HUMIDITY) {
                raw_data[1] = 'h';
                value = sensorData->humidity;
                ESP_LOGI(TAGS, "Humidity in send: %.2f", value);
            } else {
              ESP_LOGI(TAGS, "Unknown sensor type received");
              udp_status = false;
                continue;
            }

             // Convert float to bytes
            memcpy(&raw_data[2], &value, sizeof(float));

            int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock < 0) {
                ESP_LOGI(TAGS, "Socket creation failed: %d", errno);
                continue;
            }

            struct sockaddr_in dest_addr = {
                .sin_family = AF_INET,
                .sin_port = htons(8888),
                .sin_addr.s_addr = inet_addr(*sensorData->ssid_found ? IP_A_HOME : IP_A)
            };

            // Set socket to non-blocking mode
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);
         
            int err = sendto(sock, raw_data, sizeof(raw_data), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    ESP_LOGI(TAGS, "Send failed: %d", errno);
                    udp_status = false;
                } else {
                    ESP_LOGI(TAGS, "Send would block, try again later");
                    udp_status = false;
                }
            } else {
                ESP_LOGI(TAGS, "Packet sent successfully");
                udp_status = true;
            }

            close(sock);
            
           
        } else {
            ESP_LOGD(TAGS, "No event received within timeout or WiFi not connected");
        }
        vTaskDelay(pdMS_TO_TICKS(4000)); 
    
    }
    
}