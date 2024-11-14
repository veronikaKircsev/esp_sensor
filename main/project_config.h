#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#define WIFI_NAME_HOME "FRITZ!Box 7530 OK"
#define PASSWORD_HOME "48049771409576101141"
//#define WIFI_NAME_HOME "UPC4308301"
//#define PASSWORD_HOME "jk6dperdc3Qf"
#define IP_A_HOME "192.168.0.187"
#define WIFI_NAME "labs@fhv.at"
#define PASSWORD "vZDjRViutq9lSJ"
//#define IP_A_HOME "192.168.178.86"
//#define IP_A_HOME "192.168.56.1"
#define IP_A "172.21.64.224"
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