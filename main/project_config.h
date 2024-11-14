#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H


#define DATA_LENGTH 128
#define MAX_APs 20
#define GREEN 50
#define RED 50
#define WIFILED 0
#define TEMPERATURELED 1
#define HUMIDITYLED 2
#define UDPLED 3
#define DEVICE_ID 1

#include <stdbool.h>
#include <stdint.h>


extern bool humidity_status;
extern bool temperature_status;
extern bool udp_status;
extern bool wifi_status;
extern uint8_t device_id;

#endif