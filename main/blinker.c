#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "blinker.h"
#include "project_config.h"

#define BLINK_GPIO CONFIG_BLINK_GPIO

static led_strip_handle_t led_strip;

void blinking()
{
    while (1)
    {
        if(humidity_status && temperature_status && udp_status && wifi_status)
        {
            blink_led(0, 0, GREEN);  // Changed to green
        }
        else
        {
            if(!humidity_status)
                blink_led(HUMIDITYLED, RED, 0);
            if(!temperature_status)
                blink_led(TEMPERATURELED, RED, 0);
            if(!udp_status)
                blink_led(UDPLED, RED, 0);
            if(!wifi_status)
                blink_led(WIFILED, RED, 0);
        }

        led_strip_refresh(led_strip); 
        vTaskDelay(pdMS_TO_TICKS(500));  // delay for 500ms blinking
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        vTaskDelay(1000);
    }
}


void blink_led(int index, int red, int green)
{
    /* If the addressable LED is enabled */
    led_strip_set_pixel(led_strip, index, red, green, 0); 
}

void configure_led(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 25, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

