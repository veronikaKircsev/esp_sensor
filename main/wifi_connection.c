
#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "data_types.h"
#include "wifi_connection.h"
#include "project_config.h"

#define TAGI "wifi_connection"

esp_err_t wifi_err;
esp_err_t ret;
static char ip_address[16];
static bool ip_found = false;

void on_wifi_disconnect(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    ESP_LOGI(TAGI, "Wi-Fi disconnected, trying to reconnect...");
    esp_wifi_connect();
}

// Event handler for IP address acquisition
//Handler für die Verarbeitung von erfolgreichen IP-Zuweisungen nach der Verbindung.
void on_got_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    snprintf(ip_address, sizeof(ip_address), IPSTR, IP2STR(&event->ip_info.ip));
    ip_found = true;
    ESP_LOGI(TAGI, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAGI, "Got IP: %s", ip_address);
}


void wifi_connect_to_network(char *ip_address_main, bool *ssid_found, bool *wifi_connected) {
    
    wifi_config_t wifi_config;

    if (*ssid_found)
    {
        memset(&wifi_config, 0, sizeof(wifi_config));
        strcpy((char*)wifi_config.sta.ssid, WIFI_NAME_HOME);
        strcpy((char*)wifi_config.sta.password, PASSWORD_HOME);
        wifi_config.sta.scan_method = WIFI_FAST_SCAN;
        wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_config.sta.threshold.rssi = -127;
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    } else {
        memset(&wifi_config, 0, sizeof(wifi_config));
        strcpy((char*)wifi_config.sta.ssid, WIFI_NAME);
        strcpy((char*)wifi_config.sta.password, PASSWORD);
        wifi_config.sta.scan_method = WIFI_FAST_SCAN;
        wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_config.sta.threshold.rssi = -127;
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }


/*

    wifi_config_t wifi_config = {
        .sta = {
        .ssid = WIFI_NAME,
        .password = PASSWORD,
        .scan_method = WIFI_FAST_SCAN,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi =-127,
        .threshold.authmode = WIFI_AUTH_OPEN,
        },
        };
        */

   
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    wifi_err = esp_wifi_connect();
    if (wifi_err!= ESP_OK) {
        ESP_LOGI(TAGI, "Error connecting to WiFi: %s", esp_err_to_name(wifi_err));
        wifi_status = false;
        return;
    }

     int retry = 0;
    while (!ip_found && retry < 20) { // 20 * 500ms = 10 seconds timeout
        vTaskDelay(pdMS_TO_TICKS(500));
        retry++;
    }

    if (ip_found) {
        strcpy(ip_address_main, ip_address);
        ESP_LOGI(TAGI, "IP address copied: %s", ip_address_main);
        *wifi_connected = true;
        wifi_status = true;
    } else {
        ESP_LOGI(TAGI, "Failed to obtain IP address");
    }   
}

void configure_wifi(){

        ESP_LOGI(TAGI, "Example configured to connect to Wi-Fi!");
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
        esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        esp_wifi_set_default_wifi_sta_handlers();
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_start());
} 

const char* auth_mode_type(wifi_auth_mode_t auth_mode) {
    switch (auth_mode) {
        case WIFI_AUTH_OPEN:
            return "Open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA_PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2_ENTERPRISE";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3_PSK";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2_WPA3_PSK";
        case WIFI_AUTH_WAPI_PSK:
            return "WAPI_PSK";
        default:
            return "Unknown";
    }
}

void wifi_scan(bool *ssid_found) {

     wifi_scan_config_t scan_config = {
    .ssid = 0,
    .bssid = 0,
    .channel = 0,
    .show_hidden = true
  };

  
    ret = esp_wifi_scan_start(&scan_config, true);

    if (ret != ESP_OK) {
            printf("Wi-Fi Scan Start Fehler: %s\n", esp_err_to_name(ret));
            return;
        }


  wifi_ap_record_t wifi_records[MAX_APs];

  uint16_t max_records = MAX_APs;
  ret = esp_wifi_scan_get_ap_records(&max_records, wifi_records);

  if (ret != ESP_OK) {
        printf("Wi-Fi Get AP Records Fehler: %s\n", esp_err_to_name(ret));
        return;
    }

  printf("Number of Access Points Found: %d\n", max_records);
  printf("\n");
  printf("               SSID              | Channel | RSSI |   Authentication Mode \n");
  printf("***************************************************************\n");
  for (int i = 0; i < max_records; i++){
    printf("%32s | %7d | %4d | %12s\n", (char *)wifi_records[i].ssid, wifi_records[i].primary, wifi_records[i].rssi, auth_mode_type(wifi_records[i].authmode));
  printf("***************************************************************\n");
  }
  
    for (int i = 0; i < max_records; i++) {
    
            if (strcmp((char *)wifi_records[i].ssid, WIFI_NAME_HOME) == 0) {
                printf("SSID %s found\n", (char *)wifi_records[i].ssid);
                *ssid_found = true;
                break;	
            }
    }
  esp_wifi_scan_stop();
}