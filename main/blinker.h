#ifndef BLINKER_H
#define BLINKER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define BLINK_GPIO CONFIG_BLINK_GPIO

void blinking(void);

void blink_led(int index, int red, int green);
void configure_led(void);

#endif