#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include "esp_event.h"
#include "esp_wifi_types.h"

// Event handler for Wi-Fi disconnect
void on_wifi_disconnect(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// Event handler for IP address acquisition
//Handler für die Verarbeitung von erfolgreichen IP-Zuweisungen nach der Verbindung.
void on_got_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data); 


void wifi_connect_to_network(char *ip_address_main, bool *ssid_found, bool *wifi_connected); 

void configure_wifi();

const char* auth_mode_type(wifi_auth_mode_t auth_mode);

void wifi_scan(bool *ssid_found); 
#endif