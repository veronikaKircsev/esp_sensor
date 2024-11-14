
#ifndef DATA_EVENT_H
#define DATA_EVENT_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

enum SensorType {
    TEMPERATURE = 0,
    HUMIDITY = 1,
    NONE
};

struct DataEvent
{
    uint64_t timestamp;
    enum SensorType type;
};

struct SensorData{
    float temperature;
    float humidity;
    QueueHandle_t *queue;
    enum SensorType type;
    bool *ssid_found;
    bool *wifi_connected;
};


#endif