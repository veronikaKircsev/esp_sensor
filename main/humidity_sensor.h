
#ifndef HUIDITY_SENSOR_H
#define HUIDITY_SENSOR_H

void configure_master_bus(void);

void configure_devices(void);
#include "data_types.h" 

void read_sensor_data_humidity(void *pvParameters);

void check_device(void);

#endif